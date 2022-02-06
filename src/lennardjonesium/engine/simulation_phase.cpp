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

#include <variant>

#include <lennardjonesium/tools/math.hpp>
#include <lennardjonesium/tools/moving_sample.hpp>
#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/engine/simulation_phase.hpp>

namespace engine
{   
    Command
    EquilibrationPhase::evaluate(int time_step, const physics::Thermodynamics& thermodynamics)
    {
        /**
         * TODO: Could use logging here, in order to see how decisions relate to the measured
         * temperature.
         */

        // Collect temperature sample every time step
        temperatures_.push_back(thermodynamics.temperature());

        bool adjustment_needed = false;

        // Check whether adjustment is needed
        if (time_step - last_assessment_time_ >= parameters_.assessment_interval) [[unlikely]]
        {
            last_assessment_time_ = time_step;
            last_mean_temperature_ = temperatures_.statistics().mean;

            adjustment_needed = (
                tools::relative_error(last_mean_temperature_, target_temperature_)
                >= parameters_.tolerance
            );
        }

        // Check whether we are in steady state
        if ((time_step - last_adjustment_time_ >= parameters_.steady_state_time)
            && !adjustment_needed) [[unlikely]]
        {
            return PhaseComplete{};
        }

        // Check whether we have reached timeout
        if (time_step - start_time_ >= parameters_.timeout) [[unlikely]]
        {
            return AbortSimulation{};
        }

        // Send adjustment command if needed
        if (adjustment_needed) [[unlikely]]
        {
            last_adjustment_time_ = time_step;
            return SetTemperature{target_temperature_};
        }

        // If none of the above conditions were met, do nothing
        return std::monostate{};
    }
} // namespace engine

