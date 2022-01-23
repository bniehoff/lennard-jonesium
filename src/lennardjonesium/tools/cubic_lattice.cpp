/**
 * cubic_lattice.cpp
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

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <utility>
#include <ranges>

#include <Eigen/Dense>

#include <lennardjonesium/tools/aligned_generator.hpp>
#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/tools/cubic_lattice.hpp>

namespace tools
{
    CubicLattice::CubicLattice
        (int particle_count, double density, CubicLattice::UnitCell unit_cell)
        : unit_cell_{unit_cell}, particle_count_{particle_count}
    {
        // First find the total number of nonempty cells
        int nonempty_cells = std::ceil(
            static_cast<double>(particle_count) / static_cast<double>(unit_cell.cols())
        );

        // cells_per_side_ is the cube root of the smallest cube that contains this number of cells
        cells_per_side_ = std::ceil(std::cbrt(static_cast<double>(nonempty_cells)));

        // Next find the prototype density, given by the number of particles divided by volume,
        // assuming that a unit cell has unit volume
        double prototype_density = (
            static_cast<double>(particle_count) /
            static_cast<double>(cells_per_side_ * cells_per_side_ * cells_per_side_)
        );

        // scale_factor_ is then the linear scale needed to change to the desired density
        scale_factor_ = std::cbrt(prototype_density / density);
    }

    tools::aligned_generator<Eigen::Vector4d> CubicLattice::operator() ()
    {
        for (int index : std::views::iota(0, particle_count_))
        {
            /**
             * To get the coordinates of the current lattice site, we note that the index can be
             * decomposed in the following way:
             * 
             *  index = ((x * cells_per_side_ + y) * cells_per_side_ + z) * unit_cell.cols() + s
             * 
             * where (x, y, z) is the coordinate of the base point of the lattice cell, and then s
             * gives the index of the specific site within the lattice cell.  We can obtain
             * (x, y, z, s) by a sequence of quotient-remainder operations.
             */

            // We need the cast because Eigen::Index is a long int
            auto xyz_s = std::div(index, static_cast<int>(unit_cell_.cols()));
            auto xy_z = std::div(xyz_s.quot, cells_per_side_);
            auto x_y = std::div(xy_z.quot, cells_per_side_);

            // Now we can construct the lattice coordinate
            Eigen::Vector4i lattice_cell {x_y.quot, x_y.rem, xy_z.rem, 0};

            co_yield (lattice_cell.cast<double>() + unit_cell_.col(xyz_s.rem)) * scale_factor_;
        }
    }
} // namespace tools

