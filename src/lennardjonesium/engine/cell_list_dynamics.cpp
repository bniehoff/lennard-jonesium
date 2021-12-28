/**
 * cell_list_dynamics.cpp
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

#include <cassert>

#include <Eigen/Dense>
#include <boost/multi_array.hpp>

#include <lennardjonesium/engine/cell_list_dynamics.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/pairwise_force.hpp>
#include <lennardjonesium/tools/dimensions.hpp>

namespace engine
{
    // Delegate everything to the (Dimensions&, PairwiseForce*) constructor
    CellListDynamics::CellListDynamics(
        const tools::Dimensions& dimensions,
        const physics::PairwiseForce& force
    )
        : CellListDynamics::CellListDynamics(dimensions, &force)
    {}

    CellListDynamics::CellListDynamics(const tools::Dimensions& dimensions)
        : CellListDynamics::CellListDynamics(dimensions, nullptr)
    {}

    // Use the base class constructor to set the base members, then deal with the derived members
    CellListDynamics::CellListDynamics(
        const tools::Dimensions& dimensions,
        const physics::PairwiseForce *const force
    )
        : Dynamics::Dynamics(dimensions, force)
    {}

    physics::SystemState& CellListDynamics::operator() (physics::SystemState& state) const
    {
        return state;
    }
} // namespace engine

