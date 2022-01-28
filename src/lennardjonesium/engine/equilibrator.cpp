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

#include <lennardjonesium/tools/math.hpp>
#include <lennardjonesium/tools/moving_average.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/physics/transformations.hpp>
#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/engine/equilibrator.hpp>

namespace engine
{
    Equilibrator::Equilibrator(
        const Integrator& integrator,
        const Equilibrator::Parameters& parameters
    )
        : integrator_{integrator}, parameters_{parameters}
    {}

    physics::SystemState&
    Equilibrator::equilibrate_(physics::SystemState& state, double temperature)
    {
        tools::MovingAverage<double> temperature_samples(parameters_.sample_size);

        int last_measurement = 0;
        int last_adjustment = 0;

        for (int time_step = last_measurement;
            time_step < parameters_.timeout;
            time_step += parameters_.sample_period)
        {
            // Advance by sample_period time steps and take temperature sample
            state | integrator_(parameters_.sample_period);
            temperature_samples.push_back(physics::temperature(state));

            /**
             * Every measurement period, check if we are within tolerance of the desired
             * temperature.  If not, then readjust the temperature and restart the steady state
             * test.
             */
            if (time_step - last_measurement >= parameters_.measurement_period)
            {
                last_measurement = time_step;

                if (!tools::within_tolerance(
                        temperature_samples.average(), temperature, parameters_.tolerance))
                {
                    state | physics::set_temperature(temperature);
                    last_adjustment = time_step;
                }
            }

            /**
             * If we have reached the steady state period without any adjustments, then
             * equilibration is successful and we should return the state.
             */
            if (time_step - last_adjustment >= parameters_.steady_state_period)
            {
                return state;
            }
        }

        // If we reach here, then we failed to equilibrate and should throw an error.
        throw EquilibrationError();
    }
} // namespace engine
