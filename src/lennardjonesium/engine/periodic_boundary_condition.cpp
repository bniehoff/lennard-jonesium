/**
 * periodic_boundary_condition.cpp
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

#include <lennardjonesium/engine/periodic_boundary_condition.hpp>
#include <lennardjonesium/physics/system_state.hpp>

using Eigen::Array4d;
using physics::SystemState;

namespace engine
{
    PeriodicBoundaryCondition::PeriodicBoundaryCondition(const tools::BoundingBox& bounding_box)
        : bounding_box_{bounding_box}
    {}

    SystemState& PeriodicBoundaryCondition::operator() (SystemState& state) const
    {
        /**
         * This would be slightly more elegant if Eigen provided a componentwise fractional part.
         * Instead we have to subtract the integer part (floor), appropriately rescaled by the
         * size of the box.
         */

        state.positions -= (
            (state.positions.array().colwise() / bounding_box_.array()).floor().array().colwise()
            * bounding_box_.array()
        ).matrix();

        return state;
    }
} // namespace engine

