/**
 * message_buffer.hpp
 * 
 * Copyright (c) 2021-2022 Benjamin E. Niehoff
 * 
 * This file is part of Lennard-Jonesium.
 * 
 * Lennard-Jonesium is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Lennard-Jonesium is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Lennard-Jonesium.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LJ_MESSAGE_BUFFER_HPP
#define LJ_MESSAGE_BUFFER_HPP

#include <queue>
#include <deque>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <utility>
#include <memory>

namespace tools
{
    template<class T, class Alloc = std::allocator<T>>
    class MessageBuffer
    {
        /**
         * MessageBuffer allows two threads to communicate in a multi-producer, multi-consumer
         * configuration, using condition variables and mutex locks to protect the buffer during
         * operations on it.
         */

        public:
            /**
             * We hide the locking mechanisms behind the following interface:
             * 
             *  1. put(T): A producer calls put(T) to add an item to the buffer.
             * 
             *  2. std::optional<T> get(): A consumer should call get() to obtain an item from the
             *      buffer.  This call is blocking until either an item exists on the buffer, or
             *      the buffer has been closed.  If close() has been called and the buffer is
             *      empty, then get() returns std::nullopt.  Then the consumer knows that it can
             *      terminate.
             * 
             *  3. close(): The owner of the MessageBuffer should call close() after all the
             *      producers have finished.  This allows the consumers to get whatever items
             *      remain in the queue, and then terminate themselves.
             * 
             * Whoever calls close() must of course know when the producers are actually finished.
             * If there is only one producer, then it could call close() itself.  If there are
             * multiple producers, then they will need to negotiate amongst themselves.  One
             * possibility is for the ownership of the MessageBuffer to remain held by the process
             * that launches the producer and consumer threads, rather than relinquish ownership
             * via std::move().  Then one can have the following usage:
             * 
             *  {
             *      MessageBuffer buffer;
             *      std::thread producer1{...};
             *      ...
             *      std::thread producerM{...};
             *      std::thread consumer1{...};
             *      ...
             *      std::thread consumerN{...};
             * 
             *      producer1.join();
             *      ...
             *      producerM.join();
             *      buffer.close();
             *      consumer1.join();
             *      ...
             *      consumerN.join();
             *  }
             * 
             * This way, the buffer is closed after the producers finish.
             * 
             * NOTE: If close() is called before the producers finish, then some items produced
             * might not be written to the queue.
             * 
             * NOTE: If join() is called on a consumer before buffer.close() is called, then the
             * program will hang, since the consumer will be stuck waiting for the close() signal
             * which will never be sent.
             */

            void put(T);
            std::optional<T> get();
            void close();

        private:
            // The mutex is used to lock access to the buffer_ and the producer_finished_ flag
            std::mutex mutex_;

            // The Producer signals the condition variable when it makes changes
            std::condition_variable update_signal_;

            std::queue<T, std::deque<T, Alloc>> buffer_;

            // Tracks whether the buffer is still accepting messages from producers
            bool open_for_write_ = true;
    };

    template<class T, class Alloc>
    void MessageBuffer<T, Alloc>::put(T message)
    {
        // Lock the mutex within a scope
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (open_for_write_) {buffer_.push(std::move(message));}
        }

        // Signal that an item is in the buffer
        update_signal_.notify_one();
    }

    template<class T, class Alloc>
    std::optional<T> MessageBuffer<T, Alloc>::get()
    {
        // Get a unique lock in order to (possibly) wait on the condition variable
        {
            std::unique_lock<std::mutex> lock(mutex_);

            // Wait until either the buffer is nonempty, or the buffer has been closed
            update_signal_.wait(
                lock,
                [this]() {return !this->buffer_.empty() || !this->open_for_write_;}
            );

            if (!buffer_.empty())
            {
                auto message = std::move(buffer_.front());
                buffer_.pop();
                return message;
            }

            // If the buffer is empty and closed for write, return std::nullopt
            return {};
        }
    }

    template<class T, class Alloc>
    void MessageBuffer<T, Alloc>::close()
    {
        // Lock the mutex within a scope
        {
            std::lock_guard<std::mutex> lock(mutex_);
            open_for_write_ = false;
        }

        // Signal that the buffer is closed
        update_signal_.notify_all();
    }
} // namespace tools


#endif
