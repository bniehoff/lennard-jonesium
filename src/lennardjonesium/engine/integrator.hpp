/**
 * integrator.hpp
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

#ifndef LJ_INTEGRATOR_HPP
#define LJ_INTEGRATOR_HPP

#include <lennardjonesium/physics/system_state.hpp>

using physics::SystemState;

namespace engine
{
    class Integrator
    {
        /**
         * An Integrator is an operator that acts on the SystemState, evolving it forward by one
         * unit of time.  There are many different integrator strategies one could use, which will
         * be implemented in concrete derived classes.  Derived classes should override operator().
         */

        public:
            /**
             * Evolves a SystemState forward by one unit of time.  Should be given a concrete
             * implementation in derived classes.
             */
            virtual SystemState& operator() (SystemState&) = 0;

            explicit Integrator
            (
                double timestep,
                SystemState::Operator interactions = SystemState::identity_operator,
                SystemState::Operator boundary_condition = SystemState::identity_operator
            );

        protected:
            // The time step by which we will increment (assumed fixed)
            const double timestep_;

            // A state operator that computes the forces, potential energy, and virial
            SystemState::Operator interactions_;

            // A state operator that imposes the boundary condition
            SystemState::Operator boundary_condition_;
    };
} // namespace engine

#endif
