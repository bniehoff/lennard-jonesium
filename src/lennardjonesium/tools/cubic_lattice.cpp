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
#include <ranges>

#include <Eigen/Dense>

#include <lennardjonesium/draft_cpp23/generator.hpp>
#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/tools/cubic_lattice.hpp>

namespace tools
{
    CubicLattice::CubicLattice
        (int particle_count, double density, CubicLattice::UnitCell unit_cell)
        : unit_cell_{unit_cell},
          scale_{std::cbrt(unit_cell.density<double>() / density)},
          particle_count_{particle_count},
          cells_per_side_{static_cast<int>(std::ceil(
              std::cbrt(static_cast<double>(particle_count) / unit_cell.density<double>())
          ))}
    {}

    std::generator<Eigen::Vector4d> CubicLattice::operator() ()
    {
        for (int index : std::views::iota(0, particle_count_))
        {
            /**
             * To get the coordinates of the current lattice site, we note that the index can be
             * decomposed in the following way:
             * 
             *  index = ((x * cells_per_side_ + y) * cells_per_side_ + z) * unit_cell.density + s
             * 
             * where (x, y, z) is the coordinate of the base point of the lattice cell, and then s
             * gives the index of the specific site within the lattice cell.  We can obtain
             * (x, y, z, s) by a sequence of quotient-remainder operations.
             */
            auto xyz_s = std::div(index, unit_cell_.density<int>());
            auto xy_z = std::div(xyz_s.quot, cells_per_side_);
            auto x_y = std::div(xy_z.quot, cells_per_side_);

            // Now we can construct the lattice coordinate
            Eigen::Vector4i lattice_cell {x_y.quot, x_y.rem, xy_z.rem, 0};

            co_yield (
                lattice_cell.cast<double>() + unit_cell_.lattice_sites.col(xyz_s.rem)
            ) * scale_;
        }
    }
} // namespace tools

