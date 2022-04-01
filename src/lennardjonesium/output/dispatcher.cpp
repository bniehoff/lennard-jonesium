/**
 * dispatcher.cpp
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

#include <variant>

#include <lennardjonesium/tools/overloaded_visitor.hpp>
#include <lennardjonesium/output/log_message.hpp>
#include <lennardjonesium/output/sinks.hpp>
#include <lennardjonesium/output/dispatcher.hpp>

namespace output
{
    void Dispatcher::send(int time_step, LogMessage message)
    {
        // For EventSink, we flush every time, so that we will see output to the console
        // Events do not happen frequently, so this should not be a problem
        auto message_dispatcher = tools::OverloadedVisitor
        {
            [time_step, this](PhaseStartEvent message)
            {
                this->event_sink_.write(time_step, message);
                this->event_sink_.flush();
            },
            
            [time_step, this](AdjustTemperatureEvent message)
            {
                this->event_sink_.write(time_step, message);
                this->event_sink_.flush();
            },
            
            [time_step, this](RecordObservationEvent message)
            {
                this->event_sink_.write(time_step, message);
                this->event_sink_.flush();
            },
            
            [time_step, this](PhaseCompleteEvent message)
            {
                this->event_sink_.write(time_step, message);
                this->event_sink_.flush();
            },
            
            [time_step, this](AbortSimulationEvent message)
            {
                this->event_sink_.write(time_step, message);
                this->event_sink_.flush();
            },
            
            [time_step, this](ThermodynamicData message)
            {
                this->thermodynamic_sink_.write(time_step, message);
            },
            
            [time_step, this](ObservationData message)
            {
                this->observation_sink_.write(time_step, message);
            }
        };

        std::visit(message_dispatcher, message);
    }
} // namespace output
