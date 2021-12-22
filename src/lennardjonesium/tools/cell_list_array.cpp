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

    const boost::multi_array_types::size_type* CellListArray::shape()
    {
        return cell_lists_.shape();
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

        constexpr CellListArray::multi_index_type neighbors[13] = {
            {1,  1,  1},    {1,  1,  0},    {1,  1,  -1},
            {1,  0,  1},    {1,  0,  0},    {1,  0,  -1},
            {1, -1,  1},    {1, -1,  0},    {1, -1,  -1},
            {0,  1,  1},    {0,  1,  0},    {0,  1,  -1},
            {0,  0,  1}
        };

        for (cell_list_array_type::index i = 0; i < cell_lists_.shape()[0]; ++i)
            for (cell_list_array_type::index j = 0; j < cell_lists_.shape()[1]; ++j)
                for (cell_list_array_type::index k = 0; k < cell_lists_.shape()[2]; ++k)
                {
                    int i_next = (i + 1) % cell_lists_.shape()[0];
                    int j_next = (j + 1) % cell_lists_.shape()[1];
                    int k_next = (k + 1) % cell_lists_.shape()[2];
                    int j_prev = (j - 1) % cell_lists_.shape()[1];
                    int k_prev = (k - 1) % cell_lists_.shape()[2];

                    // These offsets are used to shift the neighboring element by the corresponding
                    // multiple of the box size, in the case that the next/prev wraps around.
                    int offset_i_next = (i_next > i) ? 0 : 1;
                    int offset_j_next = (j_next > j) ? 0 : 1;
                    int offset_k_next = (k_next > k) ? 0 : 1;
                    int offset_j_prev = (j_prev < j) ? 0 : -1;
                    int offset_k_prev = (k_prev < k) ? 0 : -1;

                    // Now give the 13 neighbors, packaged into a NeighborPair struct.
                    // Did not see much sense in actually writing a loop here.

                    // 1. (i+1, j+1, k+1)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i_next, j_next, k_next}),
                        Eigen::Vector4i{offset_i_next, offset_j_next, offset_k_next, 0}
                    };

                    // 2. (i+1, j+1, k)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i_next, j_next, k}),
                        Eigen::Vector4i{offset_i_next, offset_j_next, 0, 0}
                    };

                    // 3. (i+1, j+1, k-1)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i_next, j_next, k_prev}),
                        Eigen::Vector4i{offset_i_next, offset_j_next, offset_k_prev, 0}
                    };

                    // 4. (i+1, j, k+1)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i_next, j, k_next}),
                        Eigen::Vector4i{offset_i_next, 0, offset_k_next, 0}
                    };

                    // 5. (i+1, j, k)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i_next, j, k}),
                        Eigen::Vector4i{offset_i_next, 0, 0, 0}
                    };

                    // 6. (i+1, j, k-1)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i_next, j, k_prev}),
                        Eigen::Vector4i{offset_i_next, 0, offset_k_prev, 0}
                    };

                    // 7. (i+1, j-1, k+1)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i_next, j_prev, k_next}),
                        Eigen::Vector4i{offset_i_next, offset_k_prev, offset_k_next, 0}
                    };

                    // 8. (i+1, j-1, k)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i_next, j_prev, k}),
                        Eigen::Vector4i{offset_i_next, offset_k_prev, 0, 0}
                    };

                    // 9. (i+1, j-1, k-1)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i_next, j_prev, k_prev}),
                        Eigen::Vector4i{offset_i_next, offset_k_prev, offset_k_prev, 0}
                    };

                    // 10. (i, j+1, k+1)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i, j_next, k_next}),
                        Eigen::Vector4i{0, offset_j_next, offset_k_next, 0}
                    };

                    // 11. (i, j+1, k)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i, j_next, k}),
                        Eigen::Vector4i{0, offset_j_next, 0, 0}
                    };

                    // 12. (i, j+1, k-1)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i, j_next, k_prev}),
                        Eigen::Vector4i{0, offset_j_next, offset_k_prev, 0}
                    };

                    // 13. (i, j, k+1)
                    co_yield NeighborPair{
                        cell_lists_(multi_index_type{i, j, k}),
                        cell_lists_(multi_index_type{i, j, k_next}),
                        Eigen::Vector4i{0, 0, offset_k_next, 0}
                    };
                }
    }
} // namespace tools

