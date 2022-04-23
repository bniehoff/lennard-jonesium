/**
 * fixed_phases.cpp
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

#include <src/cpp/lennardjonesium/physics/measurements.hpp>
#include <src/cpp/lennardjonesium/physics/observation.hpp>
#include <src/cpp/lennardjonesium/control/command_queue.hpp>
#include <src/cpp/lennardjonesium/control/simulation_phase.hpp>

#include <tests/cpp/mock/fixed_phases.hpp>

namespace mock
{
    void SuccessPhase::evaluate(
        control::CommandQueue& command_queue,    // Output parameter
        int time_step,
        const physics::ThermodynamicMeasurement& measurement [[maybe_unused]]
    )
    {
        switch (time_step - start_time_)
        {
        case 1:
        case 3:
            command_queue.push(control::AdjustTemperature{0.5});
            command_queue.push(control::AdvanceTime{});
            break;
            
        case 5:
            command_queue.push(control::PhaseComplete{});
            break;
        
        default:
            command_queue.push(control::AdvanceTime{});
            break;
        }
    }

    void FailurePhase::evaluate(
        control::CommandQueue& command_queue,    // Output parameter
        int time_step,
        const physics::ThermodynamicMeasurement& measurement [[maybe_unused]]
    )
    {
        constexpr physics::Observation mock_observation{
            .temperature = 0.5,
            .pressure = 3.25,
            .specific_heat = 2.5,
            .diffusion_coefficient = 5.25
        };

        switch (time_step - start_time_)
        {
        case 1:
        case 3:
            command_queue.push(control::RecordObservation{mock_observation});
            command_queue.push(control::AdvanceTime{});
            break;
            
        case 5:
            command_queue.push(control::AbortSimulation{"Task failed successfully"});
            break;
        
        default:
            command_queue.push(control::AdvanceTime{});
            break;
        }
    }
} // namespace mock

