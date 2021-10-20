/**
 * boundary_condition.hpp
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

#ifndef LJ_BOUNDARY_CONDITION_HPP
#define LJ_BOUNDARY_CONDITION_HPP

#include <Eigen/Dense>

#include <lennardjonesium/physics/system_state.hpp>

using Eigen::Array4d;
using physics::SystemState;

namespace engine
{
    class BoundaryCondition
    {
        /**
         * The BoundaryCondition acts on the SystemState to impose that the particles remain in
         * a rectilinear region of space.  We use periodic boundary conditions.
         * 
         * (In principle, we could have defined some abstract BoundaryCondition and then
         * specialized it for PeriodicBoundaryConditions, but we don't intend to use any other
         * type of BoundaryCondition, so we just implement periodic ones here).
         */
        
        public:
            /**
             * Imposes the boundary conditions on the given SystemState
             */
            SystemState& operator() (SystemState&);

            /**
             * We will only use cubical bounding boxes, so we just take a single dimension
             */
            explicit BoundaryCondition(double dimension);
        
        protected:
            /**
             * We store the bounding box internally as an array type so that it can more easily
             * be used in Eigen broadcasting expressions.
             */
            const Array4d bounding_box_;
            
    };
} // namespace engine

#endif
