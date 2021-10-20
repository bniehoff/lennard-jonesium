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
         * Decrement every column in the positions matrix by the largest whole number of
         * bounding box sizes, so that the resulting position lies inside the bounding box.
         */
        
        state.positions -= (
            (state.positions.array().colwise() / bounding_box_).floor().array() * bounding_box_
        ).matrix();

        return state;
    }
} // namespace engine

