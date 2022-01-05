/**
 * bounding_box.cpp
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

#include <Eigen/Dense>

#include <lennardjonesium/tools/bounding_box.hpp>

namespace tools
{
    BoundingBox::BoundingBox(double s) : BoundingBox{{s, s, s, 1.0}}
    {}

    BoundingBox::BoundingBox(double x, double y, double z) : BoundingBox{{x, y, z, 1.0}}
    {}

    BoundingBox::BoundingBox(Eigen::Array4d dimensions) : dimensions_{dimensions}
    {}
} // namespace tools

