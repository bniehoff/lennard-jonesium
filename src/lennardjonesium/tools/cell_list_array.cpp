/**
 * cell_list_array.cpp
 * 
 * Copyright (c) 2021 Benjamin E. Niehoff
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

#include <cmath>
#include <cassert>

#include <boost/multi_array.hpp>
#include <Eigen/Dense>

#include <lennardjonesium/draft_cpp23/generator.hpp>
#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/tools/cell_list_array.hpp>

namespace tools
{
    CellListArray::CellListArray(const BoundingBox& bounding_box, double cutoff_distance)
    {
        /**
         * We need to determine how to divide the given volume into rectilinear cells.  Every
         * dimension of the cell must be at least as big as cutoff_distance.  However, the volume
         * must be divided into a whole number of cells.  Along a single axis, we can obtain
         * the appropriate number of cells via
         * 
         *      cell_count = floor(dimension / cutoff_distance)
         * 
         * NOTE: The simulation volume must be larger than the cutoff_distance along all of its
         * dimensions.
         */

        assert((
            "Simulation box size is less than the cutoff distance",
            (bounding_box.array().head<3>() > cutoff_distance).all()
        ));

        // Compute the dimensions of the cell array
        shape_ = Eigen::Array4i::Zero();

        shape_.head<3>() = (bounding_box.array().head<3>() / cutoff_distance).floor().cast<int>();

        // Resize the cell array to these dimensions
        cell_array_.resize(boost::extents[shape_[0]][shape_[1]][shape_[2]]);
    }

    const CellList& CellListArray::operator() (int x, int y, int z) const
    {return cell_array_(index_type{x, y, z});}

    CellList& CellListArray::operator() (int x, int y, int z)
    {
        // Delegate to const version
        return const_cast<CellList&>(static_cast<const CellListArray&>(*this)(x, y, z));
    }

    std::generator<CellListArray::index_type> CellListArray::cell_indices_() const
    {
        for (array_type::index i = 0; i < shape_[0]; ++i)
            for (array_type::index j = 0; j < shape_[1]; ++j)
                for (array_type::index k = 0; k < shape_[2]; ++k)
                    co_yield index_type{i, j, k};
    }

    void CellListArray::clear()
    {
        for (auto index : cell_indices_())
            cell_array_(index).clear();
    }

    std::generator<const CellList&> CellListArray::cells() const
    {
        for (auto index : cell_indices_())
            co_yield cell_array_(index);
    }

    std::generator<CellListPair> CellListArray::adjacent_pairs() const
    {
        /**
         * Each cell (i, j, k) has 26 neighbors, given by
         * 
         *      (i+1, j+1, k+1),    (i-1, j-1, k-1),
         *      (i+1, j+1, k  ),    (i-1, j-1, k  ),
         *      (i+1, j+1, k-1),    (i-1, j-1, k+1),
         *      (i+1, j,   k+1),    (i-1, j,   k-1),
         *      (i+1, j,   k  ),    (i-1, j,   k  ),
         *      (i+1, j,   k-1),    (i-1, j,   k+1),
         *      (i+1, j-1, k+1),    (i-1, j+1, k-1),
         *      (i+1, j-1, k  ),    (i-1, j+1, k  ),
         *      (i+1, j-1, k-1),    (i-1, j+1, k+1),
         *      (i,   j+1, k+1),    (i,   j-1, k-1),
         *      (i,   j+1, k  ),    (i,   j-1, k  ),
         *      (i,   j+1, k-1),    (i,   j-1, k+1),
         *      (i,   j,   k+1),    (i,   j,   k-1).
         * 
         * We must return each pair of neighboring cells exactly once, so we iterate over only
         * half of the possible neighbors, leaving the ones on the opposite side to be covered
         * by the primary iteration over all of the cells.  We choose the first column in the
         * above list, whose leading nonzero term is +1.
         */

        constexpr CellListArray::index_type neighbor_steps[13] = {
            {1,  1,  1},    {1,  1,  0},    {1,  1,  -1},
            {1,  0,  1},    {1,  0,  0},    {1,  0,  -1},
            {1, -1,  1},    {1, -1,  0},    {1, -1,  -1},
            {0,  1,  1},    {0,  1,  0},    {0,  1,  -1},
            {0,  0,  1}
        };

        // Now loop over the cell indices and neighbor steps, and compute the neighbor index and
        // lattice image
        for (const auto index : cell_indices_())
        {
            for (const auto step : neighbor_steps)
            {
                using index_array_type = Eigen::Array<array_type::index, 3, 1>;

                // Create the neighbor index
                index_type neighbor;

                // Prepare Eigen array views of index data
                Eigen::Map<const index_array_type> index_array(index.data());
                Eigen::Map<const index_array_type> step_array(step.data());
                Eigen::Map<index_array_type> neighbor_array(neighbor.data());

                // Compute the array index of the neighbor cell
                neighbor_array = index_array + step_array;

                /**
                 * Next compute the lattice image coordinates.  Since step_array can only contain
                 * {-1, 0, 1}, then the neighbor lies in a lattice image that is displaced by at
                 * most 1 in any direction from the origin.  So, simply check if we are outside
                 * of the allowed index range, and if so, the lattice image coordinate is the same
                 * as the step value in that direction.
                 */
                Eigen::Array4i lattice_image = Eigen::Array4i::Zero();

                lattice_image.head<3>() = (
                    step_array.cast<int>() *
                    (
                        neighbor_array < index_array_type::Zero() ||
                        neighbor_array >= shape_.head<3>().cast<array_type::index>()
                    ).cast<int>()
                );

                // Once we have the lattice image coordinate, use it to map the neighbor index
                // back into the appropriate bounds
                neighbor_array -= (lattice_image * shape_).head<3>().cast<array_type::index>();

                // Finally, compose the return structure
                co_yield CellListPair{lattice_image, cell_array_(index), cell_array_(neighbor)};
            }
        }
    }
} // namespace tools

