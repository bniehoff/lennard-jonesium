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
            // Takes the measurements and populates the internal fields
            const SystemState& operator() (const SystemState& state)
            {
                time_ = state | physics::time;
                kinetic_energy_ = state | physics::kinetic_energy;
                potential_energy_ = state | physics::potential_energy;
                total_energy_ = state | physics::total_energy(kinetic_energy_);
                virial_ = state | physics::virial;
                temperature_ = state | physics::temperature(kinetic_energy_);
                mean_square_displacement_ = state | physics::mean_square_displacement;

                return state;
            }

            // Get the properties measured
            double time() const {return time_;}
            double kinetic_energy() const {return kinetic_energy_;}
            double potential_energy() const {return potential_energy_;}
            double total_energy() const {return total_energy_;}
            double virial() const {return virial_;}
            double temperature() const {return temperature_;}
            double mean_square_displacement() const {return mean_square_displacement_;}

        private:
            double time_{};
            double kinetic_energy_{};
            double potential_energy_{};
            double total_energy_{};
            double virial_{};
            double temperature_{};
            double mean_square_displacement_{};
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
