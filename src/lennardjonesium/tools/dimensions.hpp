/**
 * dimensions.hpp
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

#ifndef LJ_DIMENSIONS_HPP
#define LJ_DIMENSIONS_HPP

namespace tools
{
    struct Dimensions
    {
        /**
         * Used to give bounding box dimensions.
         */

        double x;
        double y;
        double z;

        // Construct bounding box of specified dimensions
        Dimensions(double x, double y, double z);

        // Construct cubical bounding box with given side length
        explicit Dimensions(double dimension);
    };
} // namespace tools


#endif
