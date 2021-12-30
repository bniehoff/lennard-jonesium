/**
 * dynamics.cpp
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

#include <lennardjonesium/engine/dynamics.hpp>
#include <lennardjonesium/tools/dimensions.hpp>
#include <lennardjonesium/physics/pairwise_force.hpp>

namespace engine
{
    /**
     * When setting the internal Eigen::Array4d from the bounding box dimensions, we set the
     * fourth component to 1.0 rather than 0.0.  Any non-zero number is fine; the process for
     * imposing the boundary conditions involves componentwise multiplication and division by the
     * entries in the Eigen::Array4d.
     */
    Dynamics::Dynamics
        (const tools::Dimensions& dimensions, const physics::PairwiseForce& pairwise_force)
        : dimensions_{dimensions.x, dimensions.y, dimensions.z, 1.0},
          pairwise_force_{pairwise_force},
          cell_list_array_{dimensions, pairwise_force.cutoff_length()}
    {}

    physics::SystemState& Dynamics::operator() (physics::SystemState& state)
    {
        impose_boundary_conditions_(state);
        rebuild_cell_lists_(state);
        compute_forces_(state);

        return state;
    }

    void Dynamics::impose_boundary_conditions_(physics::SystemState& state)
    {
        /**
         * This would be slightly more elegant if Eigen provided a componentwise fractional part.
         * Instead we have to subtract the integer part (floor), appropriately rescaled by the
         * size of the box.
         */

        state.positions -= (
            (state.positions.array().colwise() / dimensions_).floor().array().colwise()
            * dimensions_
        ).matrix();
    }

    void Dynamics::rebuild_cell_lists_(const physics::SystemState& state)
    {
        // First compute the cell indices of every particle
        auto cell_indices = (
            state.positions.array().colwise()
            * cell_list_array_.shape().cast<double>()
            / dimensions_
        ).floor().cast<int>();

        // Next clear the existing cell list array
        cell_list_array_.clear();

        // Finally, assign each particle to its corresponding cell
        for (int i = 0; i < cell_indices.cols(); i++)
        {
            auto cell_index = cell_indices.col(i);
            cell_list_array_(cell_index.x(), cell_index.y(), cell_index.z()).push_back(i);
        }
    }

    void Dynamics::compute_forces_(physics::SystemState& state)
    {
    }
} // namespace engine

