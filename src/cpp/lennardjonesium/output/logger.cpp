/**
 * logger.cpp
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

#include <iostream>
#include <utility>
#include <thread>

#include <lennardjonesium/tools/message_buffer.hpp>
#include <lennardjonesium/output/log_message.hpp>
#include <lennardjonesium/output/sinks.hpp>
#include <lennardjonesium/output/dispatcher.hpp>
#include <lennardjonesium/output/logger.hpp>

namespace output
{
    Logger::Logger(Logger::Streams streams)
        : event_sink_{streams.event_log},
          thermodynamic_sink_{streams.thermodynamic_log},
          observation_sink_{streams.observation_log},
          snapshot_sink_{streams.snapshot_log}
    {
        // Initialize the log files
        event_sink_.write_header();
        thermodynamic_sink_.write_header();
        observation_sink_.write_header();
        snapshot_sink_.write_header();

        event_sink_.flush();
        thermodynamic_sink_.flush();
        observation_sink_.flush();
        snapshot_sink_.flush();

        // Start the consumer thread
        consumer_ = std::thread(
            [this]() {
                Dispatcher dispatcher{
                    this->event_sink_,
                    this->thermodynamic_sink_,
                    this->observation_sink_,
                    this->snapshot_sink_
                };

                auto dispatch = [&dispatcher](message_type message)
                {
                    auto [time_step, log_message] = message;
                    dispatcher.send(time_step, log_message);
                };

                // while (this->buffer_.get_and(dispatch));

                while (auto o = this->buffer_.get()) {dispatch(o.value());}

                dispatcher.flush_all();
            }
        );
    }

    void Logger::close()
    {
        // If consumer thread is running, then close the buffer
        // and wait for consumer thread to finish

        if (consumer_.joinable())
        {
            buffer_.close();
            consumer_.join();
        }
    }

    Logger::~Logger() noexcept
    {
        // Try to close the buffer
        try {close();}
        catch(...) {}
    }
} // namespace output

