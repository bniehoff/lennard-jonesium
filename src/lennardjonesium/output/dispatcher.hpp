/**
 * dispatcher.hpp
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

#ifndef LJ_DISPATCHER_HPP
#define LJ_DISPATCHER_HPP

#include <lennardjonesium/output/log_message.hpp>
#include <lennardjonesium/output/sinks.hpp>

namespace output
{
    class Dispatcher
    {
        /**
         * The Dispatcher takes a message from the message queue and sends it to the appropriate
         * Sink, based on the type of message.
         */

        public:
            void send(int time_step, LogMessage message);

            void flush_all()
            {
                event_sink_.flush();
                thermodynamic_sink_.flush();
                observation_sink_.flush();
            }

            Dispatcher(
                EventSink& event_sink,
                ThermodynamicSink& thermodynamic_sink,
                ObservationSink& observation_sink,
                SystemSnapshotSink& snapshot_sink
            )
                : event_sink_{event_sink},
                  thermodynamic_sink_{thermodynamic_sink},
                  observation_sink_{observation_sink},
                  snapshot_sink_{snapshot_sink}
            {}

        private:
            EventSink& event_sink_;
            ThermodynamicSink& thermodynamic_sink_;
            ObservationSink& observation_sink_;
            SystemSnapshotSink& snapshot_sink_;
    };
} // namespace output


#endif
