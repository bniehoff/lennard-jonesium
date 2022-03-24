/**
 * measurements.hpp
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

#ifndef LJ_MEASUREMENTS_HPP
#define LJ_MEASUREMENTS_HPP

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/derived_properties.hpp>


namespace physics
{
    // We define some useful Measurements for extracting information from the SystemState
    class ThermodynamicMeasurement
    {
        /**
         * ThermodynamicMeasurement is a Measurement captures all of the useful instantaneous
         * thermodynamic information about the state, which can be used to make decisions or
         * further statistical observations.
         */

        public:
            struct Result
            {
                /**
                 * It is useful to package the result of the measurement into a struct, so that
                 * it can be passed between functions as a single piece of data.
                 */

                double time{};
                double kinetic_energy{};
                double potential_energy{};
                double total_energy{};
                double virial{};
                double temperature{};
                double mean_square_displacement{};
            };

            // Takes the measurements and populates the internal fields
            const SystemState& operator() (const SystemState& state)
            {
                result_.time = state | physics::time;
                result_.kinetic_energy = state | physics::kinetic_energy;
                result_.potential_energy = state | physics::potential_energy;
                result_.total_energy = state | physics::total_energy(result_.kinetic_energy);
                result_.virial = state | physics::virial;
                result_.temperature = state | physics::temperature(result_.kinetic_energy);
                result_.mean_square_displacement = state | physics::mean_square_displacement;

                return state;
            }

            // Get the result of measurement
            const Result& result() const {return result_;}

        private:
            Result result_;
    };

    class KinematicMeasurement
    {
        /**
         * KinematicMeasurement is used to collect data about the state in order to verify
         * conservation laws.  This includes total momentum, total force, energies, etc.
         */

        // TODO: Fill this in
    };
    
} // namespace physics


#endif
