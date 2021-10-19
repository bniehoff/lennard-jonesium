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

#include <lennardjonesium/engine/system_state.hpp>

namespace engine {
    class Integrator {
        /**
         * An Integrator acts on the SystemState via the method evolve_forward(), which increments
         * the SystemState by one time step.  There are many different integrator strategies one
         * could use, which will be implemented in concrete derived classes.
         */
        protected:
            // The time step by which we will increment (assumed fixed)
            const double timestep_;

            // A state operator that computes the forces, potential energy, and virial
            SystemState::Operator compute_interactions_;

            // A state operator that imposes the boundary conditions
            SystemState::Operator impose_boundary_conditions_;
        
        public:
            Integrator(
                double timestep,
                SystemState::Operator compute_interactions = SystemState::identity_operator,
                SystemState::Operator impose_boundary_conditions = SystemState::identity_operator
            );

            // Evolves time by one time step.  Must be given concrete implementation.
            virtual SystemState& evolve_forward(SystemState&) = 0;
    };
}

#endif
