/**
 * simulation.cpp
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

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/transformations.hpp>
#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/control/command_queue.hpp>
#include <lennardjonesium/control/simulation_phase.hpp>
#include <lennardjonesium/control/simulation.hpp>

namespace
{
    // Define an overload template for visiting the Command variant
    template<class... Lambdas>
    struct Overloaded : Lambdas... { using Lambdas::operator()...; };
} // namespace


namespace control
{
    physics::SystemState& Simulation::operator() (physics::SystemState& state)
    {
        // Clock for counting the global time
        int time_step = 0;

        // Measuring device to get the instantaneous thermodynamic information
        physics::ThermodynamicMeasurement measurement;

        // Initialize the first SimulationPhase
        simulation_phases_.front()->set_start_time(time_step);

        // Log phase start event

        // Prepare the CommandQueue which will control execution
        CommandQueue command_queue;
        command_queue.push(AdvanceTime{});

        // Create the visitor object once
        auto command_interpreter = Overloaded
        {
            [&](const AdvanceTime& command)
            {
                state | this->integrator_(command.time_steps) | measurement;
                time_step += command.time_steps;
                this->simulation_phases_.front()->evaluate(command_queue, time_step, measurement);
            },

            [&](const RecordObservation& command [[maybe_unused]])
            {
                // Send observation to file
                // Log observation event
            },

            [&](const AdjustTemperature& command)
            {
                state | physics::set_temperature(command.temperature);

                // Log temperature adjustment event
            },

            [&](const PhaseComplete& command [[maybe_unused]])
            {
                this->simulation_phases_.pop();

                // Log phase success event

                // Initialize the new phase with the time step
                if (!this->simulation_phases_.empty())
                {
                    this->simulation_phases_.front()->set_start_time(time_step);

                    // Log phase start event
                }
            },

            [&](const AbortSimulation& command [[maybe_unused]])
            {
                // Log abort event
            }
        };
        
        // Main simulation loop
        while (!command_queue.empty())
        {
            std::visit(command_interpreter, command_queue.front());
            command_queue.pop();
        }

        return state;
    }
} // namespace control
