/**
 * equilibrator.cpp
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

#include <ranges>

#include <lennardjonesium/tools/math.hpp>
#include <lennardjonesium/tools/moving_sample.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/derived_properties.hpp>
#include <lennardjonesium/physics/transformations.hpp>
#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/engine/equilibrator.hpp>

namespace engine
{
    Equilibrator::Equilibrator(
        const Integrator& integrator,
        Equilibrator::Parameters parameters
    )
        : integrator_{integrator}, parameters_{parameters}
    {}

    physics::SystemState&
    Equilibrator::equilibrate_(physics::SystemState& state, double temperature)
    {
        /**
         * TODO: It would be useful to have logging in this function.
         * 
         * TODO: Also, we need to move the time loop elsewhere, there should be a single time loop
         * that the equilibration functions somehow "register" into.  This would be the cleanest
         * way to keep a global time_step counter.
         */

        tools::MovingSample<double> temperature_samples(parameters_.sample_size);

        for (int last_adjustment = 0; int time_step : std::views::iota(0, parameters_.timeout))
        {
            // Advance by one time step
            state | integrator_;

            // Collect temperature samples
            if (time_step % parameters_.sample_interval == 0)
            {
                temperature_samples.push_back(physics::temperature(state));
            }
            
            // Check whether an adjustment is needed
            if (time_step % parameters_.assessment_interval == 0)
            {
                double measured_temperature = temperature_samples.statistics().mean;

                if (tools::relative_error(measured_temperature, temperature)
                    >= parameters_.tolerance)
                {
                    state | physics::set_temperature(temperature);
                    last_adjustment = time_step;
                }
            }

            // Check whether we are in steady state
            if (time_step - last_adjustment >= parameters_.steady_state_time)
            {
                return state;
            }
        }

        // If we reach here, then we failed to equilibrate and should throw an error.
        throw EquilibrationError();
    }
} // namespace engine
