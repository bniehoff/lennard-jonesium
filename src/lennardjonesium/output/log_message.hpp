/**
 * log_message.hpp
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

#ifndef LJ_LOG_MESSAGE_HPP
#define LJ_LOG_MESSAGE_HPP

#include <string>
#include <variant>

#include <Eigen/Dense>

#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/physics/observation.hpp>
#include <lennardjonesium/physics/system_state.hpp>

namespace output
{
    /**
     * There are several types of LogMessages which will be combined into a std::variant.  The
     * Dispatcher will be responsible for taking these messages from the message buffer and sending
     * them to the appropriate destination.
     */

    struct PhaseStartEvent
    {
        std::string name;
    };

    struct AdjustTemperatureEvent
    {
        double temperature;
    };

    struct RecordObservationEvent {};

    struct PhaseCompleteEvent
    {
        std::string name;
    };

    struct AbortSimulationEvent
    {
        std::string reason;
    };

    struct ThermodynamicData
    {
        physics::ThermodynamicMeasurement::Result data;
    };

    struct ObservationData
    {
        physics::Observation data;
    };

    struct SystemSnapshot
    {
        /**
         * Rather than capture the entire SystemState, we capture only the parts that will be
         * printed to the file.  This way the size of the LogMessage variant does not grow bigger
         * than 64B (so it will fit in a single cache line, if that is important).
         */

        Eigen::Matrix4Xd positions;
        Eigen::Matrix4Xd velocities;
        Eigen::Matrix4Xd forces;
    };

    using LogMessage = std::variant<
        PhaseStartEvent,
        AdjustTemperatureEvent,
        RecordObservationEvent,
        PhaseCompleteEvent,
        AbortSimulationEvent,
        ThermodynamicData,
        ObservationData,
        SystemSnapshot
    >;
} // namespace output


#endif
