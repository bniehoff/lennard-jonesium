/**
 * command_queue.hpp
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

#ifndef LJ_COMMAND_QUEUE_HPP
#define LJ_COMMAND_QUEUE_HPP

#include <queue>
#include <variant>

#include <lennardjonesium/physics/observation.hpp>

namespace control
{
    /**
     * We use the Command pattern to implement communication between the SimulationPhase and the
     * Simulation.  A Command is a std::variant among the following types.  When the Simulation
     * receives a Command, it will execute the appropriate action.
     * 
     * We use std::variant and delegate the interpretation of these commands to the Simulation, so
     * that SimulationPhase does not acquire a dependency on Simulation in order to effectively
     * control it.
     */

    // Use the integrator to advance by some number of time steps
    struct AdvanceTime
    {
        int time_steps = 1;
    };

    // Record an observation result computed from statistical data
    struct RecordObservation
    {
        physics::Observation observation;
    };

    // Adjust the temperature of the system
    struct AdjustTemperature
    {
        double temperature;
    };

    // On success, end this phase and move on to next
    struct PhaseComplete {};

    // On failure, end simulation
    struct AbortSimulation {};

    // The Command variant itself
    using Command = std::variant<
        AdvanceTime,
        RecordObservation,
        AdjustTemperature,
        PhaseComplete,
        AbortSimulation
    >;

    // Commands will be pushed onto a CommandQueue and then popped when executed
    using CommandQueue = std::queue<Command>;
} // namespace control


#endif
