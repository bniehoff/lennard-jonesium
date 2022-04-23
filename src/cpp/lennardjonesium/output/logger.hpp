/**
 * logger.hpp
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

#ifndef LJ_LOGGER_HPP
#define LJ_LOGGER_HPP

#include <iostream>
#include <utility>
#include <thread>

#include <boost/iostreams/device/null.hpp>
#include <boost/iostreams/stream.hpp>

#include <lennardjonesium/tools/message_buffer.hpp>
#include <lennardjonesium/output/log_message.hpp>
#include <lennardjonesium/output/sinks.hpp>

namespace output
{
    class Logger
    {
        /**
         * The Logger prints the relevant simulation data to output streams.  It is not a flexible
         * and generic logging system like the Boost logging library, nor is it intended for
         * diagnostic output.  Its main purpose is to record the actual physics data that is
         * produced in the course of simulation.
         * 
         * NOTE: The files it produces are meant to be permanent (not rotated like system logs).
         * The Logger accepts std::ostream& arguments to its constructor; the caller is responsible
         * for configuring these streams and making sure they point to opened files.  The caller
         * is also responsible for closing the streams afterward.
         */

        public:
            struct Streams
            {
                std::ostream& event_log;
                std::ostream& thermodynamic_log;
                std::ostream& observation_log;
                std::ostream& snapshot_log;
            };

            Logger(Streams);

            // Used by producer thread to send log messages, which will be dispatched to the
            // appropriate destination
            void log(int time_step, LogMessage message) {buffer_.put({time_step, message});}

            // Call close() after producer threads are finished, this clears the message buffer
            // and terminates consumer thread (optional)
            // Note that there is no way to reopen logging
            void close();

            // If close() was not called, perform the same operations when we go out of scope
            ~Logger() noexcept;
        
        private:
            EventSink event_sink_;
            ThermodynamicSink thermodynamic_sink_;
            ObservationSink observation_sink_;
            SystemSnapshotSink snapshot_sink_;
            tools::MessageBuffer<std::pair<int, LogMessage>> buffer_;
            std::thread consumer_;
    };
} // namespace output


#endif
