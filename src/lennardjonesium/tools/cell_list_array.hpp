/**
 * cell_list_array.hpp
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

#ifndef LJ_CELL_LIST_ARRAY_HPP
#define LJ_CELL_LIST_ARRAY_HPP

#include <vector>

#include <boost/multi_array.hpp>
#include <Eigen/Dense>

#include <lennardjonesium/draft_cpp23/generator.hpp>
#include <lennardjonesium/tools/bounding_box.hpp>

namespace tools
{
    /**
     * A CellList is used to keep track of which particles are in a particular cell, or rectilinear
     * subregion of the simulation box.
     */
    using CellList = std::vector<int>;

    struct CellListPair
    {
        /**
         * CellListPair contains references to two adjacent cells, as well as an array that gives
         * the coordinates of the lattice image of the periodic bounding box to which the second
         * CellList belongs (this is to account for "neighboring cells" which wrap around the
         * boundary).  So, e.g., if lattice_image is {0, 0, 1, 0}, then the second CellList should
         * be interpreted as belonging to a copy of the simulation box that is displaced in the
         * z direction by 1 (multiple of the box's z dimension).
         */

        Eigen::Array4i lattice_image;

        const CellList& first;
        const CellList& second;
    };

    class CellListArray
    {
        /**
         * CellListArray maintains a structure of Cell Lists which are used to find pairs of
         * particles that are within the cutoff distance of each other.  A CellList is a list
         * of indices into the SystemState.
         * 
         * Note that we do not depend on the SystemState, and CellListArray is responsible only
         * for maintaining the CellLists and not for populating them in the first place.  The only
         * information needed is the shape (i.e. integer dimensions) of the multidimensional array
         * of cells, which can be deduced from the cutoff distance and the full dimensions of the
         * simulation box.
         */

        public:
            CellListArray(const BoundingBox& bounding_box, double cutoff_distance);

            // Access an element
            CellList& operator() (int, int, int);

            // Access an element on a const object (not sure if needed)
            const CellList& operator() (int, int, int) const;

            // Get the shape of the multidimensional array
            const Eigen::Array4i shape() const {return shape_;};

            // Clear the elements of the array
            void clear();

            // Generator to traverse the individual cells
            std::generator<const CellList&> cells() const;

            // Generator to traverse adjacent pairs of cells (including periodic wrap around)
            std::generator<CellListPair> adjacent_pairs() const;
        
        private:
            using array_type = boost::multi_array<CellList, 3>;
            using index_type = boost::array<array_type::index, 3>;

            std::generator<index_type> cell_indices_() const;

            // We store the cell lists internally in a boost::multi_array
            array_type cell_array_;

            // We also store the shape
            Eigen::Array4i shape_;
    };
} // namespace tools


#endif
