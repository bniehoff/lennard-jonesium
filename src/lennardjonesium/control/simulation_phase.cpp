/**
 * simulation_phase.cpp
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

#include <memory>
#include <variant>

#include <lennardjonesium/tools/math.hpp>
#include <lennardjonesium/tools/moving_sample.hpp>
#include <lennardjonesium/physics/measurements.hpp>

#include <lennardjonesium/control/simulation_phase.hpp>

namespace control
{   
    std::vector<Command>
    EquilibrationPhase::evaluate
        (int time_step, const physics::ThermodynamicMeasurement& measurement)
    {
        // Prepare the sequence of commands to be returned
        std::vector<Command> commands;

        // Collect temperature sample every time step
        temperature_analyzer_.collect(measurement);

        // Check whether adjustment is needed
        if (time_step - last_adjustment_check_time_
            >= equilibration_parameters_.adjustment_interval) [[unlikely]]
        {
            last_adjustment_check_time_ = time_step;
            last_temperature_ = temperature_analyzer_.result();

            if (tools::relative_error(last_temperature_, system_parameters_.temperature)
                >= equilibration_parameters_.tolerance) [[unlikely]]
            {
                last_adjustment_time_ = time_step;
                commands.push_back(AdjustTemperature{system_parameters_.temperature});
            }
        }

        // Check whether we are in steady state
        if (time_step - last_adjustment_time_ >= equilibration_parameters_.steady_state_time)
            [[unlikely]]
        {
            commands.push_back(PhaseComplete{});
        }

        // Check whether we have reached timeout
        if (time_step - start_time_ >= equilibration_parameters_.timeout) [[unlikely]]
        {
            commands.push_back(AbortSimulation{});
        }

        return commands;
    }

    std::vector<Command>
    ObservationPhase::evaluate
        (int time_step, const physics::ThermodynamicMeasurement& measurement)
    {
        // Prepare the sequence of commands to be returned
        std::vector<Command> commands;

        // Collect relevant data every time step
        thermodynamic_analyzer_.collect(measurement);

        // Check whether we should compute an Observation
        if (time_step - last_observation_time_ >= observation_parameters_.observation_interval)
            [[unlikely]]
        {
            last_observation_time_ = time_step;

            auto observation = thermodynamic_analyzer_.result();

            // Check whether the temperature has drifted too far from the nominal value
            if (tools::relative_error(observation.temperature, system_parameters_.temperature)
                >= observation_parameters_.tolerance) [[unlikely]]
            {
                commands.push_back(AbortSimulation{});
            }
            else
            {
                ++observation_count_;
                commands.push_back(RecordObservation{observation});
            }
        }

        // Check whether we have collected enough Observations
        if (observation_count_ >= observation_parameters_.observation_count) [[unlikely]]
        {
            commands.push_back(PhaseComplete{});
        }
        
        return commands;
    }
} // namespace control

