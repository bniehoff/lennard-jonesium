/**
 * worker.hpp
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

#include <fmt/core.h>

#include <lennardjonesium/tools/message_buffer.hpp>
#include <lennardjonesium/api/worker.hpp>

namespace api
{
    void Worker::launch()
    {
        std::thread producer
        {
            [this]() {
                for (int i : std::views::iota(0, 10))
                {
                    this->message_buffer_.put(
                        fmt::format("Count: {}", i)
                    );
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }

                this->message_buffer_.close();
            }
        };

        producer.detach();
    }

    std::pair<bool, std::string> Worker::get()
    {
        auto message = message_buffer_.get();

        if (message.has_value())
        {
            return {true, message.value()};
        }
        else
        {
            return {false, ""};
        }
    }
} // namespace api

