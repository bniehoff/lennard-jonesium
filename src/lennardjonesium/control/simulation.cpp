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
#include <queue>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/transformations.hpp>
#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/engine/integrator.hpp>
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

        // Prepare a queue of Commands to be received from the SimulationPhase
        std::queue<Command> commands;

        // Signal to tell us when to exit the loop
        bool running = true;

        // Create the visitor object once
        auto command_interpreter = Overloaded
        {
            [&](const RecordObservation& command [[maybe_unused]])
            {
                // Need to use the _previous_ time step here, since time has already incremented
            },

            [&](const AdjustTemperature& command)
            {
                state | physics::set_temperature(command.temperature);
            },

            [&](const PhaseComplete& command [[maybe_unused]])
            {
                this->simulation_phases_.pop();

                if (this->simulation_phases_.empty())
                {
                    // End the simulation if there are no more phases
                    running = false;
                }
                else
                {
                    // Initialize the new phase with the time step and start a new command queue
                    this->simulation_phases_.front()->set_start_time(time_step);
                    commands = {};
                }
            },

            [&](const AbortSimulation& command [[maybe_unused]])
            {
                running = false;
            }
        };
        
        // Main simulation loop
        while (running)
        {
            /**
             * We use a two-state loop structure that either advances the simulation by one time
             * step OR processes the commands from the SimulationPhase.  This allows us to process
             * a queue of multiple commands, while having only one level of loop nesting.
             * 
             * This makes it much easier to break out of the loop when the simulation ends.  It also
             * helps make sure that we fully process one command at a time (and, for example, not
             * process any additional commands after an AbortSimulation command, even if there may
             * be some in the queue).
             */

            if (commands.empty())
            {
                state | integrator_ | measurement;
                commands = simulation_phases_.front()->evaluate(time_step, measurement);
                ++time_step;
            }
            else
            {
                std::visit(command_interpreter, commands.front());
                commands.pop();
            }
        }

        return state;
    }
} // namespace control
