/**
 * bounding_box.hpp
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

#ifndef LJ_BOUNDING_BOX_HPP
#define LJ_BOUNDING_BOX_HPP

#include <Eigen/Dense>

namespace tools
{
    class BoundingBox
    {
        /**
         * BoundingBox will describe the size of the simulation box in three dimensions.  It is
         * just a thin wrapper around an Eigen::Vector4d which gives some convenient constructors
         * and helps encapsulate the 4d vectors being used to represent the system, despite it
         * being only 3d.
         * 
         * Note that BoundingBox is always used _multiplicatively_ (e.g., to check whether something
         * is in the box, one can use `position.array() / box.array()`).  So, the 4th component of
         * the array representation is always 1.0.
         */

        public:
            // Construct a cubical BoundingBox with the same side length along all 3 dimensions
            explicit BoundingBox(double side_length);

            // Construct a rectilinear BoundingBox with the given side lengths
            BoundingBox(double x, double y, double z);

            // Get the array representation to use in Eigen expressions
            Eigen::Array4d array() const {return dimensions_;};
        
        private:
            // Construct directly from an Eigen 4-array
            BoundingBox(Eigen::Array4d dimensions);

            Eigen::Array4d dimensions_;
    };
} // namespace tools


#endif
