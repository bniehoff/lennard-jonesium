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
#include <lennardjonesium/physics/operator.hpp>

using physics::SystemState;
using physics::Operator;

namespace engine
{
    template <Operator interaction_type, Operator boundary_condition_type>
    class Integrator
    {
        /**
         * An Integrator is an operator that acts on the SystemState, evolving it forward by one
         * unit of time.  There are many different integrator strategies one could use, which will
         * be implemented in concrete derived classes.  Derived classes should override operator().
         */

        protected:
            // The time step by which we will increment (assumed fixed)
            const double timestep_;

            // A state operator that computes the forces, potential energy, and virial
            interaction_type interaction_;

            // A state operator that imposes the boundary condition
            boundary_condition_type boundary_condition_;

        public:
            /**
             * Evolves a SystemState forward by one unit of time.  Should be given a concrete
             * implementation in derived classes.
             */
            virtual SystemState& operator() (SystemState&) = 0;

            explicit Integrator(double timestep)
                : timestep_(timestep),
                  interaction_(physics::identity_operator),
                  boundary_condition_(physics::identity_operator)
            {}

            Integrator
            (
                double timestep,
                interaction_type interaction,
                boundary_condition_type boundary_condition
            )
                : timestep_(timestep),
                  interaction_(interaction),
                  boundary_condition_(boundary_condition)
            {}
    };

    // Template argument deduction guide
    Integrator(double)
        -> Integrator<decltype(physics::identity_operator), decltype(physics::identity_operator)>;
} // namespace engine

#endif
