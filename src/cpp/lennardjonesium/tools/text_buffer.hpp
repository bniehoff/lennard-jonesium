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
#include <boost/iostreams/operations.hpp>

#include <lennardjonesium/tools/message_buffer.hpp>

namespace tools
{
    class TextBuffer
    {
        /**
         * TextBuffer is a wrapper around MessageBuffer<std::string>, and provides a simpler
         * interface which is easier to call from Cython.  In particular, we wrap the get() method
         * with a method called read(), which simply returns the empty string when we reach the
         * end of the message queue.  To make sure that none of the actual messages is the empty
         * string, we wrap put() in a write() method which automatically discards empty strings.
         * 
         * In theory, TextBuffer should allow multiple producers and multiple consumers; however,
         * using multiple consumers would be rather silly.
         * 
         * Usage:
         *      write():        Push a string to the buffer.
         * 
         *      close():        Close the write() end of the stream; the read() end will continue
         *                      to pop elements from the queue until the queue is empty.
         * 
         *      read():         Pop one string from the queue and return it.
         */

        public:
            void write(const std::string& s)
            {
                if (!s.empty())
                {
                    buffer_.put(s);
                }
            }

            void close() {buffer_.close();}
            
            std::string read()
            {
                if (auto o = buffer_.get())
                {
                    return o.value();
                }
                else
                {
                    return {};
                }
            }
        
        private:
            tools::MessageBuffer<std::string> buffer_;
    };

    class TextBufferFilter : public boost::iostreams::line_filter
    {
        /**
         * TextBufferFilter is a wrapper for TextBuffer that models the boost::iostreams::filter
         * concept.  This allows it to grab lines from a stream and push them to a TextBuffer
         * object.
         */

        public:
            struct category
                : boost::iostreams::dual_use,
                boost::iostreams::filter_tag,
                boost::iostreams::multichar_tag,
                // boost::iostreams::flushable_tag,
                boost::iostreams::closable_tag
            { };

            explicit TextBufferFilter(TextBuffer& buffer) : buffer_{buffer} {}

            // template<typename Sink>
            // bool flush(Sink& snk)
            // {
            //     std::cout << "TextBufferFilter flushed\n";
            //     return boost::iostreams::flush(snk);
            // }

            template<typename Sink>
            void close(Sink& snk, BOOST_IOS::openmode which)
            {
                buffer_.close();
                boost::iostreams::line_filter::close(snk, which);
            }
        
        private:
            TextBuffer& buffer_;

            virtual string_type do_filter(const string_type& line) override
            {
                // boost::iostreams::line_filter removes the newline character
                buffer_.write(line);
                return line;
            };
    };
} // namespace tools


#endif
