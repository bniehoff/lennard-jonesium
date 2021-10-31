/**
 * cell_list_dynamics.hpp
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

#ifndef LJ_CELL_LIST_DYNAMICS_HPP
#define LJ_CELL_LIST_DYNAMICS_HPP

#include <vector>

#include <Eigen/Dense>

#include <boost/multi_array.hpp>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/pairwise_force.hpp>
#include <lennardjonesium/engine/dynamics.hpp>

namespace engine
{
    class CellListDynamics : public Dynamics
    {
        /**
         * A CellListDynamics implements the dynamics of short-range forces by using cell lists.
         */

        // Although we're calling it a "list", we use a contiguous vector
        typedef std::vector<int> cell_list;

        public:
            // Imposes the boundary conditions and calculates the forces of the system
            virtual physics::SystemState& operator() (physics::SystemState&) const override;
            
            // Creates a CellListDynamics with a given bounding box and pairwise force
            CellListDynamics(const Dimensions&, const physics::PairwiseForce&);

            // Creates a CellListDynamics from a bounding box only, with no forces
            CellListDynamics(const Dimensions&);
        
        protected:
            // The 3-dimensional array of cell lists
            boost::multi_array<cell_list, 3> cell_lists_;

            // Internal constructor that directly assigns to variables
            CellListDynamics(const Dimensions&, const physics::PairwiseForce *const);
    };
} // namespace engine


#endif
