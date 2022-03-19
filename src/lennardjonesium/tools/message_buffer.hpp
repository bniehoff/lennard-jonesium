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
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace tools
{
    template<class T>
    class MessageBuffer
    {
        /**
         * MessageBuffer allows two threads to communicate in a single-producer, single-consumer
         * configuration, using condition variables and mutex locks to protect the buffer during
         * operations on it.
         */

        public:
            /**
             * We hide the locking mechanisms behind this interface.  The Producer should use
             * put(T) to place an item in the buffer, and call end() when it has no more items to
             * place (this allows the Consumer to know when the Producer is finished).  The Consumer
             * should call get() to obtain an item from the buffer.  The call to get() will block
             * until an item is obtained; or, if the Producer has called end(), then get() will
             * return std::nullopt.  In this way, the Consumer can deduce that the buffer is empty
             * and will receive no further input, so that the Consumer can terminate.
             */

            void put(T);
            void end();
            std::optional<T> get();

        private:
            // The mutex is used to lock access to the buffer_ and the producer_finished_ flag
            std::mutex mutex_;

            // The Producer signals the condition variable when it makes changes
            std::condition_variable producer_updated_;

            std::queue<T> buffer_;

            bool producer_finished_ = false;
    };

    template<class T>
    void MessageBuffer<T>::put(T message)
    {
        // Lock the mutex within a scope
        {
            std::lock_guard<std::mutex> lock(mutex_);
            buffer_.push(message);
        }

        // Signal that an item is in the buffer
        producer_updated_.notify_one();
    }

    template<class T>
    void MessageBuffer<T>::end()
    {
        // Lock the mutex within a scope
        {
            std::lock_guard<std::mutex> lock(mutex_);
            producer_finished_ = true;
        }

        // Signal that producer_finished_ has been set
        producer_updated_.notify_one();
    }

    template<class T>
    std::optional<T> MessageBuffer<T>::get()
    {
        // Get a unique lock in order to (possibly) wait on the condition variable
        {
            std::unique_lock<std::mutex> lock(mutex_);

            // Wait until either the buffer is nonempty, or producer_finished_ is set
            producer_updated_.wait(
                lock,
                [&]() {return !this->buffer_.empty() || this->producer_finished_;}
            );

            if (!buffer_.empty())
            {
                auto message = buffer_.front()
                buffer_.pop();
                return message;
            }

            // If the buffer is empty and the Producer is finished, return std::nullopt
            return {};
        }
    }
} // namespace tools


#endif
