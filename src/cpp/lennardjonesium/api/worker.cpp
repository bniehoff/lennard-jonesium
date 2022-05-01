/**
 * worker.cpp
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
#include <utility>
#include <thread>
#include <ranges>
#include <chrono>
#include <iostream>
#include <exception>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/null.hpp>

#include <lennardjonesium/tools/text_buffer.hpp>
#include <lennardjonesium/api/worker.hpp>


namespace api
{   
    void Worker::launch(int count)
    {
        for (int i : std::views::iota(0, count))
        {
            fmt::print(std::cout, "Count: {}\n", i);
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void AsyncWorker::launch(int count)
    {
        wait();

        buffer_ = std::make_unique<tools::TextBuffer>();
        out_stream_.push(tools::TextBufferFilter(*buffer_));
        out_stream_.push(boost::iostreams::null_sink());

        producer_ = std::jthread(
            [this, count]() {
                for (int i : std::views::iota(0, count))
                {
                    // this->buffer_->write(fmt::format("Count: {}\n", i));
                    fmt::print(this->out_stream_, "Count: {}\n", i);
                    this->out_stream_.flush();
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }

                this->out_stream_.reset();
                // this->buffer_->close();
            }
        );
    }

    void AsyncWorker::wait()
    {
        if (producer_.joinable())
        {
            producer_.join();

            buffer_.reset();
        }
    }

    bool AsyncWorker::eof()
    {
        if (buffer_)
        {
            return buffer_->eof();
        }
        else
        {
            return true;
        }
    }

    std::string AsyncWorker::read()
    {
        if (buffer_)
        {
            return buffer_->read();
        }
        else
        {
            return {};
        }
    }

    AsyncWorker::~AsyncWorker()
    {
        try {wait();}
        catch (...) {}
    }
} // namespace api
