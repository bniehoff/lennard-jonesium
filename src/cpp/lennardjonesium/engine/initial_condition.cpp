/**
 * initial_condition.cpp
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

#include <random>
#include <ranges>

#include <Eigen/Dense>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/tools/cubic_lattice.hpp>
#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/transformations.hpp>
#include <lennardjonesium/engine/initial_condition.hpp>

namespace engine
{
    InitialCondition::InitialCondition(
        tools::SystemParameters system_parameters,
        tools::CubicLattice::UnitCell unit_cell,
        std::random_device::result_type seed
    )
        : InitialCondition::InitialCondition(
            system_parameters,
            tools::CubicLattice{system_parameters, unit_cell},
            seed
        )
    {}

    InitialCondition::InitialCondition(
        tools::SystemParameters system_parameters,
        tools::CubicLattice cubic_lattice,
        std::random_device::result_type seed
    )
        : system_parameters_{system_parameters},
          bounding_box_{cubic_lattice.bounding_box()},
          system_state_{system_parameters.particle_count},
          seed_{seed}
    {
        /**
         * Now we set the initial positions of all the particles by placing them on the lattice
         * sites determined by the CubicLattice.
         * 
         * Unfortunately, std::views::zip does not yet exist, so we need to keep track of an index
         * manually in order to know which particle should be assigned where.
         */
        for (int index = 0; auto position : cubic_lattice())
        {
            system_state_.positions.col(index) = position;
            ++index;
        }

        /**
         * Next, we choose the initial velocities from a Maxwell-Boltzmann distribution.  This is
         * just a normal distribution with mean 0 and variance equal to the temperature.
         */
        random_number_engine_type gen{seed_};
        std::normal_distribution<> maxwell_boltzmann_distribution{
            0,
            std::sqrt(system_parameters_.temperature)
        };

        // The individual velocity components are all independent, so we treat them as a 1d array
        for (auto& velocity_component : system_state_.velocities.topRows<3>().reshaped())
        {
            velocity_component = maxwell_boltzmann_distribution(gen);
        }

        /**
         * We still need to zero the linear and angular momentum, which will in turn affect
         * the temperature (since kinetic energy is not invariant under changes of frame).
         * 
         * It is important to specifically use the angular momentum about the center of mass,
         * otherwise the two operations of zeroing linear and angular momentum do not commute.
         */

        auto center_of_mass = physics::center_of_mass(system_state_);

        system_state_
            | physics::zero_momentum()
            | physics::zero_angular_momentum(center_of_mass)
            | physics::set_temperature(system_parameters_.temperature);
        
        // Now the initial state is set up.
    }
} // namespace engine

