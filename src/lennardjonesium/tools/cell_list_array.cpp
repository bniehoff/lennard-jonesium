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
#include <ranges>
#include <array>

#include <Eigen/Dense>

#include <lennardjonesium/draft_cpp23/generator.hpp>
#include <lennardjonesium/tools/dimensions.hpp>
#include <lennardjonesium/tools/cell_list_array.hpp>

namespace tools
{   
    CellListArray::CellListArray(tools::Dimensions dimensions, double cutoff_length)
    {
        /**
         * We need to determine how to divide the given volume into rectilinear cells.  Every
         * dimension of the cell must be at least as big as cutoff_length.  However, the volume
         * must be divided into a whole number of cells.  Along a single axis, we can obtain
         * the appropriate number of cells via
         * 
         *      cell_count = floor(dimension / cutoff_length)
         * 
         * NOTE: The simulation volume must be larger than the cutoff_length along all of its
         * dimensions.  Otherwise it is not valid to treat a force with this cutoff_length as a
         * short-range force.
         */

        assert((
            "Simulation box size is less than the cutoff length",
            (dimensions.x > cutoff_length &&
             dimensions.y > cutoff_length &&
             dimensions.z > cutoff_length)
        ));

        // Obtain the cell counts along the three axes
        auto x = static_cast<int>(std::floor(dimensions.x / cutoff_length));
        auto y = static_cast<int>(std::floor(dimensions.y / cutoff_length));
        auto z = static_cast<int>(std::floor(dimensions.z / cutoff_length));


        // Resize the cell lists array according to the cell counts
        cell_lists_.resize(boost::extents[x][y][z]);
    }

    const CellList& CellListArray::operator() (int x, int y, int z) const
    {
        return cell_lists_(multi_index_type{x, y, z});
    }

    CellList& CellListArray::operator() (int x, int y, int z)
    {
        // Delegate to const version
        return const_cast<CellList&>(static_cast<const CellListArray&>(*this)(x, y, z));
    }

    const Eigen::Array4i CellListArray::shape() const
    {
        /**
         * The entries of cell_lists_.shape() are of type unsigned long.  Boost is expecting very
         * large arrays.  In our case this will not happen, so the cast to int should be safe.
         */
        return Eigen::Array4i{
            static_cast<int>(cell_lists_.shape()[0]),
            static_cast<int>(cell_lists_.shape()[1]),
            static_cast<int>(cell_lists_.shape()[2]),
            0
        };
    }

    std::generator<CellList&> CellListArray::cell_view()
    {
        // It is important to iterate over the innermost index first, to take advantage of storage
        // order w.r.t. locality of reference.
        for (cell_list_array_type::index i = 0; i < cell_lists_.shape()[0]; ++i)
            for (cell_list_array_type::index j = 0; j < cell_lists_.shape()[1]; ++j)
                for (cell_list_array_type::index k = 0; k < cell_lists_.shape()[2]; ++k)
                    co_yield cell_lists_(multi_index_type{i, j, k});
    }

    std::generator<NeighborPair&&> CellListArray::neighbor_view()
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

        constexpr CellListArray::multi_index_type displacements[13] = {
            {1,  1,  1},    {1,  1,  0},    {1,  1,  -1},
            {1,  0,  1},    {1,  0,  0},    {1,  0,  -1},
            {1, -1,  1},    {1, -1,  0},    {1, -1,  -1},
            {0,  1,  1},    {0,  1,  0},    {0,  1,  -1},
            {0,  0,  1}
        };

        for (cell_list_array_type::index i = 0; i < cell_lists_.shape()[0]; ++i)
            for (cell_list_array_type::index j = 0; j < cell_lists_.shape()[1]; ++j)
                for (cell_list_array_type::index k = 0; k < cell_lists_.shape()[2]; ++k)
                    for (auto displacement : displacements)
                        co_yield get_neighbor_pair_(multi_index_type{i, j, k}, displacement);
    }

    void CellListArray::clear()
    {
        for (auto& cell_list : cell_view())
            cell_list.clear();
    }

    NeighborPair CellListArray::get_neighbor_pair_
        (const multi_index_type index, const multi_index_type displacement)
    {
        multi_index_type neighbor_index = index;
        Eigen::Vector4i offset {0, 0, 0, 0};

        /**
         * Note: We assume that displacement is one of {-1, 0, 1}.  We cannot use the modulo
         * operator % naively because it does not have the correct behavior for negative
         * displacements.
         */

        for (int d = 0; d < 3; d++)
        {
            // First compute the displaced index without imposing modulo arithmetic
            neighbor_index[d] += displacement[d];

            // Since the displacement can only be {-1, 0, 1}, we can compute the offset without
            // the modulo operator and without branching.  If we are outside of the correct index
            // range, then shift in the direction of displacement.
            offset[d] = displacement[d] * (
                (neighbor_index[d] < 0) || (neighbor_index[d] >= cell_lists_.shape()[d])
            );

            // Once we have the offset, use it to find the correct neighbor index in the index range
            neighbor_index[d] -= offset[d] * cell_lists_.shape()[d];
        }

        return NeighborPair{cell_lists_(index), cell_lists_(neighbor_index), offset};
    }
} // namespace tools

