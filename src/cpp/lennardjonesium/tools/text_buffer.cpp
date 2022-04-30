/**
 * text_buffer.hpp
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

#include <string>
#include <string_view>
#include <sstream>
#include <mutex>
#include <condition_variable>

#include <lennardjonesium/tools/text_buffer.hpp>

namespace
{
    bool is_empty(std::stringstream& ss)
    {
        return ss.tellp() == 0;
    }
} // namespace


namespace tools
{
    void TextBuffer::write(std::string_view text)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!end_of_write_) {buffer_ << text;}
        }

        update_signal_.notify_one();
    }

    std::string TextBuffer::read()
    {
        // Need a unique lock to wait for condition variable
        std::unique_lock<std::mutex> lock(mutex_);

        // Wait until there is text in the buffer or eof is set
        update_signal_.wait(
            lock,
            [this]() {return !is_empty(this->buffer_) || this->end_of_write_;}
        );

        // Since we are about to empty the buffer (or it is empty already), then if end_of_write_
        // is set, we can set end_of_read_
        end_of_read_ = end_of_write_;

        // If there is something to read, return it
        if (!is_empty(buffer_))
        {
            std::string text = buffer_.str();
            buffer_.str("");
            return text;
        }
        else
        {
            // Otherwise, return the empty string
            return {};
        }
    }

    bool TextBuffer::eof()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return end_of_read_;
    }

    void TextBuffer::close()
    {
        // Set the end_of_write_ flag
        {
            std::lock_guard<std::mutex> lock(mutex_);
            end_of_write_ = true;
        }

        // Notify all of change
        update_signal_.notify_all();
    }
} // namespace tools

