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

#include <vector>
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
            // The mutex is used to lock access to the input_ buffer and the input_finished_ flag
            std::mutex input_mutex_;

            // The Producer signals the condition variable when it makes changes
            std::condition_variable input_available_;

            /**
             * We will use double buffering to implement the queue, assuming the Producer will get
             * ahead of the Consumer.
             */
            std::vector<T> input_;
            std::vector<T> output_;

            // We track our current position in the output_ vector with an iterator
            std::vector<T>::iterator output_iterator_ = output_.begin();

            bool input_finished_ = false;
    };

    template<class T>
    void MessageBuffer<T>::put(T message)
    {
        // Lock the mutex within a scope
        {
            std::lock_guard<std::mutex> lock(input_mutex_);
            input_.push_back(message);
        }

        // Signal that an item is in the buffer
        input_available_.notify_one();
    }

    template<class T>
    void MessageBuffer<T>::end()
    {
        // Lock the mutex within a scope
        {
            std::lock_guard<std::mutex> lock(input_mutex_);
            input_finished_ = true;
        }

        // We also need to signal when changing input_finished_
        input_available_.notify_one();
    }

    template<class T>
    std::optional<T> MessageBuffer<T>::get()
    {
        std::optional<T> message = std::nullopt;

        if (output_iterator_ == output_.end())
        {
            /**
             * Here we have exhausted the output buffer.  We should check whether this is anything
             * more to do.  If the input buffer is nonempty, then we can swap the buffers.  If it
             * is empty, but the Producer is not yet finished, then we should wait until the
             * Producer either pushes another message or signals that it is finished.
             */

            // Get a unique lock in order to (possibly) wait on the condition variable
            {
                std::unique_lock<std::mutex> lock(input_mutex_);

                if (input_.empty() && !input_finished_)
                {
                    // There are no messages available, but the Producer has not indicated they
                    // are finished.  Therefore, wait for new messages.
                    input_available_.wait(lock);
                }
                
                if (!input_.empty())
                {
                    // There are messages available, so we can swap the buffers
                    output_.clear();
                    std::swap(input_, output_);
                    output_iterator_ = output_.begin();
                }

                // If the input buffer is empty and input_finished_ = true, then we fall through
            }
        }

        /**
         * This condition must be separate from `if (output_iterator_ == output_end())` rather than
         * given as an `else` clause, because after waiting on the condition variable, the buffers
         * may have been swapped, in which case there may be items in the output buffer to read. 
         */

        if (output_iterator_ < output_.end())
        {
            // Read an item from the output buffer
            message = *output_iterator_;
            ++output_iterator_;
        }

        return message;
    }
} // namespace tools


#endif
