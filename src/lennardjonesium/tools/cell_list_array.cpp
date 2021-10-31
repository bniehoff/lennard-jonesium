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
            (dimensions.x < cutoff_length ||
             dimensions.y < cutoff_length ||
             dimensions.z < cutoff_length)
        ));

        // Obtain the cell counts along the three axes
        int x = std::floor(dimensions.x / cutoff_length);
        int y = std::floor(dimensions.y / cutoff_length);
        int z = std::floor(dimensions.z / cutoff_length);

        // Resize the cell lists array according to the cell counts
        cell_lists_.resize(boost::extents[x][y][z]);
    }

    const CellList& CellListArray::operator() (int x, int y, int z) const
    {
        return cell_lists_(multi_index_{x, y, z});
    }

    CellList& CellListArray::operator() (int x, int y, int z)
    {
        // Delegate to const version
        return const_cast<CellList&>(static_cast<const CellListArray&>(*this)(x, y, z));
    }
} // namespace tools

