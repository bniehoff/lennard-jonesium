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
#include <lennardjonesium/engine/interaction.hpp>
#include <lennardjonesium/engine/boundary_condition.hpp>

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
            virtual physics::SystemState& operator() (physics::SystemState&) const = 0;

            /**
             * Create a "default" integrator with the given timestep, and no interactions or
             * boundary conditions.
             */
            explicit Integrator(double timestep);

            // Create an integrator with the given time step, interaction, and boundary condition
            Integrator(double timestep, Interaction&, BoundaryCondition&);

            // Create from addresses, useful if we want to set just one of the entries to nullptr
            Integrator(double timestep, const Interaction*, const BoundaryCondition*);

        protected:
            // The time step by which we will increment (assumed fixed)
            const double timestep_;

            /**
             * For interactions or boundary conditions, we use nullptr to indicate that these
             * steps will be skipped.  Calling them requires vtable lookup, which normally
             * has no impact, since they must loop over a large number of particles anyway.  But
             * when we want to skip imposing forces or boundary conditions, the vtable lookup
             * becomes significant, so it is nicer to just check for nullptr rather than calling
             * a "no-op" function that requires vtable lookup to find.
             */

            // A state operator that computes the forces, potential energy, and virial
            const Interaction* interaction_;

            //A state operator that imposes the boundary condition
            const BoundaryCondition* boundary_condition_;
    };
} // namespace engine

#endif
