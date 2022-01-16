/**
 * initial_condition.hpp
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

#ifndef LJ_INITIAL_CONDITION_HPP
#define LJ_INITIAL_CONDITION_HPP

#include <random>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/physics/system_state.hpp>

namespace engine
{
    class InitialCondition
    {
        /**
         * InitialCondition is responsible for setting up the initial SystemState and BoundingBox.
         * This includes putting the particles in their initial positions, and giving them a
         * velocity distribution that corresponds to the desired temperature.
         * 
         * We treat the particle_count as approximate, with the rule that we will create a state
         * with AT LEAST this particle count.  In practice, we choose the lowest perfect cube which
         * is at least as big as particle_count, so that we can create a perfect cubical lattice
         * of particles.  This simplifies some computations regarding the angular momentum of the
         * system.
         * 
         * We do not enforce any particular method of choosing the initial seed.  If not provided,
         * we use the default one.  The caller is responsible for determining their own method of
         * choosing a seed, either via the system time or std::random_device, etc.
         */

        public:
            InitialCondition(int particle_count, double density, double temperature);

            InitialCondition(int particle_count, double density, double temperature,
                             std::random_device::result_type seed);
            
            // These return by value so that the original InitialCondition will not be modified
            tools::BoundingBox bounding_box();
            physics::SystemState system_state();

            int particle_count();
            double density();
            double temperature();

            std::random_device::result_type seed();
        
        private:
            struct Parameters_
            {
                /**
                 * The Parameters object is a helper struct to allow us to do some computation on
                 * the input parameters and repackage them into a type for overload resolution.
                 * This allows us to pre-compute some necessary information in the initializer list
                 * before constructing the BoundingBox and the SystemState; in particular, it
                 * allows both objects to be stack-allocated.
                 * 
                 * For the initial particle placement, we will use a face-centered cubic crystal
                 * structure.  Given the input parameters, we need to calculate how many lattice
                 * cells (along one dimension) are required, as well as the linear size of each
                 * lattice cell.
                 */

                int lattice_size;
                double cell_size;

                double density;
                double temperature;
                std::random_device::result_type seed;

                Parameters_(int particle_count, double density, double temperature,
                            std::random_device::result_type seed);
            };

            // Delegate constructor
            InitialCondition(Parameters_);

            // Data members
            tools::BoundingBox bounding_box_;
            physics::SystemState system_state_;

            double density_;
            double temperature_;
            std::random_device::result_type seed_;
    };
} // namespace engine


#endif
