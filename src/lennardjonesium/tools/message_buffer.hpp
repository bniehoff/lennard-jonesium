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

#include <cassert>
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
             *  1. MessageBuffer(int producer_count): We must tell the MessageBuffer how many
             *      producers are expected to generate output, otherwise the consumers have no
             *      way of knowing when all the producers are finished.  The default producer
             *      count is 1.
             * 
             *  2. put(T): A producer calls put(T) to add an item to the buffer.
             * 
             *  3. end(): A producer must call end() when it is finished.  When all the producers
             *      have called end(), the consumer thread will then know that it has no further
             *      messages to wait for, and can terminate once it finishes processing the
             *      remaining items in the buffer.
             * 
             *  4. std::optional<T> get(): A consumer should call get() to obtain an item from the
             *      buffer.  This call is blocking until either an item exists on the buffer, or
             *      all the producers have finished.  If all producers have finished and the buffer
             *      is empty, then get() returns std::nullopt.  Then the consumer knows that it can
             *      terminate.
             */

            void put(T);
            void end();
            std::optional<T> get();

            MessageBuffer() = default;
            explicit MessageBuffer(int producer_count) : producers_pending_{producer_count} {}

        private:
            // The mutex is used to lock access to the buffer_ and the producer_finished_ flag
            std::mutex mutex_;

            // The Producer signals the condition variable when it makes changes
            std::condition_variable update_signal_;

            std::queue<T, std::deque<T, Alloc>> buffer_;

            /**
             * The producers have no way of knowing how many more producers are expected in the
             * future unless we say explicitly.  When a producer calls end(), we decrement the
             * pending count.  When it reaches zero, we know that we are not waiting for any
             * further input.
             */
            int producers_pending_ = 1;
    };

    template<class T, class Alloc>
    void MessageBuffer<T, Alloc>::put(T message)
    {
        // Lock the mutex within a scope
        {
            std::lock_guard<std::mutex> lock(mutex_);
            buffer_.push(std::move(message));
        }

        // Signal that an item is in the buffer
        update_signal_.notify_one();
    }

    template<class T, class Alloc>
    void MessageBuffer<T, Alloc>::end()
    {
        // Lock the mutex within a scope
        {
            std::lock_guard<std::mutex> lock(mutex_);
            --producers_pending_;
            assert(producers_pending_ >= 0 && "Tried to end more producers than expected");
        }

        // Signal that the pending count has changed
        update_signal_.notify_all();
    }

    template<class T, class Alloc>
    std::optional<T> MessageBuffer<T, Alloc>::get()
    {
        // Get a unique lock in order to (possibly) wait on the condition variable
        {
            std::unique_lock<std::mutex> lock(mutex_);

            // Wait until either the buffer is nonempty, or there are no producers active
            update_signal_.wait(
                lock,
                [&]() {return !this->buffer_.empty() || (this->producers_pending_ == 0);}
            );

            if (!buffer_.empty())
            {
                auto message = std::move(buffer_.front());
                buffer_.pop();
                return message;
            }

            // If the buffer is empty and there are no producers pending, return std::nullopt
            return {};
        }
    }
} // namespace tools


#endif
