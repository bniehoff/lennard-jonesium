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

    /**
     * Now we need to define generators
     */

    std::generator<CellList&> CellListArray::cell_view()
    {
        for (cell_list_array_type::index i = 0; i < cell_lists_.shape()[0]; ++i)
            for (cell_list_array_type::index j = 0; j < cell_lists_.shape()[1]; ++j)
                for (cell_list_array_type::index k = 0; k < cell_lists_.shape()[2]; ++k)
                    co_yield cell_lists_(multi_index_type{i, j, k});
    }

    // std::generator<NeighborPair&> CellListArray::neighbor_view()
    // {
    //     // TODO
    // }
} // namespace tools

