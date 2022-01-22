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
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/initial_condition.hpp>

namespace engine
{
    InitialCondition::InitialCondition(
        int particle_count,
        double density,
        double temperature,
        tools::CubicLattice::UnitCell unit_cell,
        std::random_device::result_type seed
    )
        : InitialCondition::InitialCondition(
            particle_count,
            density,
            temperature,
            tools::CubicLattice{particle_count, density, unit_cell},
            seed
        )
    {}

    InitialCondition::InitialCondition(
        int particle_count,
        double density,
        double temperature,
        tools::CubicLattice cubic_lattice,
        std::random_device::result_type seed
    )
        : bounding_box_{cubic_lattice.bounding_box()},
          system_state_{particle_count},
          density_{density},
          temperature_{temperature},
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
        std::normal_distribution<> maxwell_boltzmann_distribution{0, std::sqrt(temperature_)};

        // The individual velocity components are all independent, so we treat them as a 1d array
        for (auto& velocity_component : system_state_.velocities.reshaped())
        {
            velocity_component = maxwell_boltzmann_distribution(gen);
        }

        /**
         * TODO: We still need to zero the linear and angular momentum, which will in turn affect
         * the temperature (since kinetic energy is not invariant under changes of frame).
         * We can also include a final temperature rescaling step, but temperature rescaling needs
         * to be defined outside this class, since it will also be used elsewhere.
         */
    }
} // namespace engine

