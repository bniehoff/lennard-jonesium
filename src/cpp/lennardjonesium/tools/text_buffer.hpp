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

#ifndef LJ_TEXT_BUFFER_HPP
#define LJ_TEXT_BUFFER_HPP

#include <string>
#include <string_view>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <memory>

#include <boost/iostreams/filter/line.hpp>

namespace tools
{
    class TextBuffer
    {
        /**
         * TextBuffer is similar to MessageBuffer, but specialized for multi-line text.
         * 
         * TextBuffer allows one thread to write into a block text buffer and another thread to
         * read from it.  Rather than reading one line at a time, a read operation grabs the entire
         * contents.
         * 
         * In theory, TextBuffer should allow multiple producers and multiple consumers; however,
         * using multiple consumers would be rather silly.
         * 
         * Usage:
         *      write():        Push a string to the buffer.  Returns number of characters written.
         * 
         *      close():        Close the write() end of the stream; eof will be indicated to the
         *                      read() end after the stream is empty.
         * 
         *      read():         Read the contents of the buffer.  This call is blocking, until
         *                      there are contents to be returned (so if it returns the empty
         *                      string, then we have reached eof).
         * 
         *      eof():          Returns whether we have reached the end of the stream.
         */

        public:
            void write(std::string_view);
            void close();
            std::string read();
            bool eof();
        
        private:
            std::mutex mutex_;
            std::condition_variable update_signal_;
            std::stringstream stream_;
            bool end_of_write_ = false;
            bool end_of_read_ = false;
    };

    class TextBufferFilter : public boost::iostreams::line_filter
    {
        /**
         * TextBufferFilter is a wrapper for TextBuffer that models the boost::iostreams::filter
         * concept.  This allows it to grab lines from a stream and push them to a TextBuffer
         * object.
         */

        public:
            explicit TextBufferFilter(TextBuffer& buffer) : buffer_{buffer} {}

            template<typename Sink>
            void close(Sink& snk, BOOST_IOS::openmode which)
            {
                buffer_.close();
                boost::iostreams::line_filter::close(snk, which);
            }
        
        private:
            TextBuffer& buffer_;

            virtual string_type do_filter(const string_type&) override;
    };
} // namespace tools


#endif
