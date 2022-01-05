/**
 * boundary_condition.hpp
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

#ifndef LJ_BOUNDARY_CONDITION_HPP
#define LJ_BOUNDARY_CONDITION_HPP

#include <lennardjonesium/physics/system_state.hpp>

namespace engine
{
    class BoundaryCondition
    {
        /**
         * The BoundaryCondition acts on the SystemState to impose that the particles remain in
         * a region of space.
         */
        
        public:
            // Imposes the boundary conditions on the system state
            virtual physics::SystemState& operator() (physics::SystemState&) const = 0;
            
    };

    // Single instance of anonymous subclass that imposes no boundary conditions
    inline const class : public BoundaryCondition
    {
        public:
            virtual physics::SystemState& operator() (physics::SystemState& s) const override
            {return s;}
    } null_boundary_condition;
} // namespace engine

#endif
