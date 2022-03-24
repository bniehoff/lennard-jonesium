/**
 * message.hpp
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

#ifndef LJ_MESSAGE_HPP
#define LJ_MESSAGE_HPP

#include <string>
#include <variant>

#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/physics/observation.hpp>
#include <lennardjonesium/tools/message_buffer.hpp>

namespace output
{
    /**
     * We define the format we expect to use for Messages which will be logged to various files.
     * They will be combined into a std::variant so that we can switch on the type of message.
     * 
     * Note: We could have used std::pair<int, T> for each of these, but then it wouldn't be so
     * clear that the int is intended to be the time step.
     */

    struct EventMessage
    {
        int time_step;
        std::string description;
    };

    struct ObservationMessage
    {
        int time_step;
        physics::Observation observation;
    };

    struct ThermodynamicMessage
    {
        int time_step;
        physics::ThermodynamicMeasurement::Result result;
    };

    using Message = std::variant<
        EventMessage,
        ObservationMessage,
        ThermodynamicMessage
    >;

    // The Buffer class itself is just a type alias
    using Buffer = tools::MessageBuffer<Message>;
} // namespace output


#endif
