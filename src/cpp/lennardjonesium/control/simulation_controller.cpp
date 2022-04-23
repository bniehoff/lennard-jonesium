/**
 * simulation_controller.cpp
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
#include <algorithm>

#include <lennardjonesium/tools/overloaded_visitor.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/transformations.hpp>
#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/output/log_message.hpp>
#include <lennardjonesium/output/logger.hpp>
#include <lennardjonesium/control/command_queue.hpp>
#include <lennardjonesium/control/simulation_phase.hpp>
#include <lennardjonesium/control/simulation_controller.hpp>

namespace control
{
    physics::SystemState& SimulationController::operator() (physics::SystemState& state)
    {
        // Clock for counting the global time
        int time_step = 0;

        // Measuring device to get the instantaneous thermodynamic information
        physics::ThermodynamicMeasurement measurement;

        // Initialize the first SimulationPhase
        simulation_phases_.front()->set_start_time(time_step);

        // Log phase start event
        logger_.log(time_step, output::PhaseStartEvent{simulation_phases_.front()->name()});

        // Prepare the CommandQueue which will control execution
        CommandQueue command_queue;
        command_queue.push(AdvanceTime{});

        // Create the visitor object once
        auto command_interpreter = tools::OverloadedVisitor
        {
            [&](const AdvanceTime& command)
            {
                state | (*this->integrator_)(command.time_steps) | measurement;

                // Log the measurement
                this->logger_.log(time_step, output::ThermodynamicData{measurement.result()});

                time_step += command.time_steps;
                this->simulation_phases_.front()->evaluate(command_queue, time_step, measurement);
            },

            [&](const RecordObservation& command)
            {
                // Send observation to file
                this->logger_.log(time_step, output::ObservationData{command.observation});

                // Log observation event
                this->logger_.log(time_step, output::RecordObservationEvent{});
            },

            [&](const AdjustTemperature& command)
            {
                state | physics::set_temperature(command.temperature);

                // Log temperature adjustment event
                this->logger_.log(time_step, output::AdjustTemperatureEvent{command.temperature});
            },

            [&](const PhaseComplete& command [[maybe_unused]])
            {
                // Log phase complete event
                this->logger_.log(time_step, output::PhaseCompleteEvent{
                    this->simulation_phases_.front()->name()
                });
                
                this->simulation_phases_.pop();

                if (this->simulation_phases_.empty())
                {
                    // If we are finished, then record a snapshot
                    this->logger_.log(time_step, output::SystemSnapshot{
                        .positions = state.positions,
                        .velocities = state.velocities,
                        .forces = state.forces
                    });
                }
                else
                {
                    // Otherwise, initialise the new phase and continue simulation
                    this->simulation_phases_.front()->set_start_time(time_step);
                    
                    // Prepare the command queue
                    command_queue.push(AdvanceTime{});

                    // Log phase start event
                    this->logger_.log(time_step, output::PhaseStartEvent{
                        this->simulation_phases_.front()->name()
                    });
                }
            },

            [&](const AbortSimulation& command)
            {
                // Log abort event
                this->logger_.log(time_step, output::AbortSimulationEvent{command.reason});

                // Log snapshot in case we would like it for diagnostic reasons
                this->logger_.log(time_step, output::SystemSnapshot{
                    .positions = state.positions,
                    .velocities = state.velocities,
                    .forces = state.forces
                });
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
