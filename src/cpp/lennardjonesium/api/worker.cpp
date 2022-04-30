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

        buffer_ = std::make_unique<buffer_type>();

        producer_ = std::jthread(
            [this, count]() {
                for (int i : std::views::iota(0, count))
                {
                    this->buffer_->write(fmt::format("Count: {}\n", i));
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }

                this->buffer_->close();
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

    // std::pair<bool, std::string> AsyncWorker::get()
    // {
    //     if (buffer_)
    //     {
    //         auto message = buffer_->get();
    //         if (message.has_value())
    //         {
    //             return {true, message.value()};
    //         }
    //         else
    //         {
    //             return {false, ""};
    //         }
    //     }
    //     else
    //     {
    //         return {false, ""};
    //     }
    // }

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
