/**
 * periodic_boundary_condition.hpp
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

#ifndef LJ_PERIODIC_BOUNDARY_CONDITION_HPP
#define LJ_PERIODIC_BOUNDARY_CONDITION_HPP

#include <Eigen/Dense>

#include <lennardjonesium/engine/boundary_condition.hpp>
#include <lennardjonesium/physics/system_state.hpp>

namespace engine
{
    class PeriodicBoundaryCondition : public BoundaryCondition
    {
        /**
         * PeriodicBoundaryConditions keep the system in a rectilinear box by causing particles
         * that leave the box on one side to appear on the opposite side.
         */
        
        public:
            // Imposes the boundary conditions on the system state
            virtual physics::SystemState& operator() (physics::SystemState&) const override;

            // Create a rectilinear box with given x, y, z dimensions
            PeriodicBoundaryCondition(double x, double y, double z);

            // Create a cubical box with the same dimension in all three directions
            explicit PeriodicBoundaryCondition(double dimension);
        
        protected:
            /**
             * We store the bounding box internally as an array type so that it can more easily
             * be used in Eigen broadcasting expressions.
             */
            const Eigen::Array4d bounding_box_;

            // Directly assign the bounding box from a given array; the 4th element must be 1.0
            PeriodicBoundaryCondition(Eigen::Array4d);
            
    };
} // namespace engine

#endif
