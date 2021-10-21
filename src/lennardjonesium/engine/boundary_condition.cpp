/**
 * boundary_condition.cpp
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

#include <Eigen/Dense>

#include <lennardjonesium/engine/boundary_condition.hpp>
#include <lennardjonesium/physics/system_state.hpp>

using Eigen::Array4d;
using physics::SystemState;

namespace engine
{
    BoundaryCondition::BoundaryCondition(double dimension)
        /**
         * The .w() component must be nonzero in order to prevent division by zero in expressions.
         * However, the exact value is not important.
         */
        : bounding_box_{dimension, dimension, dimension, 1.0}
    {}

    SystemState& BoundaryCondition::operator() (SystemState& state)
    {
        /**
         * This would be slightly more elegant if Eigen provided a componentwise fractional part.
         * Instead we have to subtract the integer part (floor), appropriately rescaled by the
         * size of the box.
         * 
         * Note that this code is generic for rectilinear bounding boxes, although the constructor
         * above only creates cubical boxes.
         */

        state.positions -= (
            (state.positions.array().colwise() / bounding_box_).floor().array().colwise()
            * bounding_box_
        ).matrix();

        return state;
    }
} // namespace engine

