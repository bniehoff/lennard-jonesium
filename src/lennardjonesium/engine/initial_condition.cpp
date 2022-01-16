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
#include <cmath>
#include <array>
#include <ranges>

#include <Eigen/Dense>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/initial_condition.hpp>

using random_number_generator = std::mt19937;

// We use a face-centered cubic lattice which has 4 sites per cell, at the following locations
constexpr int lattice_sites_per_cell{4};
constexpr std::array<double, 4> lattice_sites[lattice_sites_per_cell] = {
    {0.0, 0.0, 0.0, 0.0},
    {0.5, 0.0, 0.0, 0.0},
    {0.0, 0.5, 0.0, 0.0},
    {0.0, 0.0, 0.5, 0.0}
};

// We will want to treat the above values as Eigen vectors
using vector4d_view = Eigen::Map<const Eigen::Vector4d>;

namespace engine
{
    InitialCondition::InitialCondition(int particle_count, double density, double temperature)
        : InitialCondition::InitialCondition(
            particle_count, density, temperature, random_number_generator::default_seed
        )
    {}

    InitialCondition::InitialCondition
    (int particle_count, double density, double temperature, std::random_device::result_type seed)
        : InitialCondition::InitialCondition(
            InitialCondition::Parameters_{particle_count, density, temperature, seed}
        )
    {}

    InitialCondition::Parameters_::Parameters_
    (int particle_count, double density, double temperature, std::random_device::result_type seed)
        : density{density}, temperature{temperature}, seed{seed}
    {
        /**
         * First obtain the number of lattice sites along a single side.  We obtain this by solving
         * 
         *      lattice_sites_per_cell * (lattice_size)^3 = particle_count
         * 
         * for lattice_size, and rounding up (so that the total number of particles is at least
         * particle_count).
         */
        lattice_size = std::ceil(
            std::cbrt(
                static_cast<double>(particle_count) / static_cast<double>(lattice_sites_per_cell)
            )
        );

        /**
         * Next we obtain the cell_size by requiring that
         * 
         *      lattice_sites_per_cell / (cell_size)^3 = density
         * 
         * and solving this for cell_size.
         */
        cell_size = std::cbrt(static_cast<double>(lattice_sites_per_cell) / density);

        // Now all of the members of the Parameters_ object have been set
    }

    InitialCondition::InitialCondition(InitialCondition::Parameters_ p)
        : bounding_box_{p.cell_size * static_cast<double>(p.lattice_size)},
          system_state_{lattice_sites_per_cell * p.lattice_size * p.lattice_size * p.lattice_size},
          density_{p.density},
          temperature_{p.temperature},
          seed_{p.seed}
    {
        /**
         * Now we set the initial positions of all the particles by placing them on the lattice
         * sites of a face-centered-cubic crystal lattice.
         */
        const Eigen::Array4d unit_cell{p.cell_size, p.cell_size, p.cell_size, 1.0};

        /**
         * We need the indices i, j, k, site_index to construct the various position vectors,
         * so we are stuck either using 4 nested loops, or a single loop up the the particle_count,
         * but then decomposing this loop index into (i, j, k, site_index) with a series of modulo
         * operations.  It is much easier to read with nested loops.
         */
        for (int i : std::views::iota(0, p.lattice_size))
        {
            for (int j : std::views::iota(0, p.lattice_size))
            {
                for (int k : std::views::iota(0, p.lattice_size))
                {
                    for (int site_index : std::views::iota(0, lattice_sites_per_cell))
                    {
                        // First compute the index of the particle we will modify
                        int particle_index = site_index + lattice_sites_per_cell * (
                            k + p.lattice_size * (j + p.lattice_size * i)
                        );

                        // Next compute and set the particle's position
                        Eigen::Vector4i cell_base{i, j, k, 0};
                        vector4d_view lattice_site_offset{lattice_sites[site_index].data()};

                        system_state_.positions.col(particle_index) = (
                            (cell_base.cast<double>() + lattice_site_offset).array() * unit_cell
                        ).matrix();
                    }
                }
            }
        }

        /**
         * Next, we choose the initial velocities from a Maxwell-Boltzmann distribution.  This is
         * just a normal distribution with mean 0 and variance equal to the temperature.
         */
        random_number_generator gen{p.seed};
        std::normal_distribution<> maxwell_boltzmann_distribution{0, std::sqrt(p.temperature)};

        // The individual velocity components are all independent, so we treat them as a 1d array
        for (auto& velocity_component : system_state_.velocities.reshaped())
        {
            velocity_component = maxwell_boltzmann_distribution(gen);
        }
    }
} // namespace engine

