/**
 * velocity_verlet_integrator.hpp
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

#ifndef LJ_VELOCITY_VERLET_INTEGRATOR_HPP
#define LJ_VELOCITY_VERLET_INTEGRATOR_HPP

#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/physics/system_state.hpp>

using physics::SystemState;

namespace engine
{
    template <Operator interaction_type, Operator boundary_condition_type>
    class VelocityVerletIntegrator : public Integrator<interaction_type, boundary_condition_type>
    {
        public:
            // Should be able to inherit constructor without problems
            // using Integrator<interaction_type, boundary_condition_type>::Integrator;

            // We cannot inherit the base constructor as above, instead we must delegate to it:
            explicit VelocityVerletIntegrator(double timestep)
                : Integrator<interaction_type, boundary_condition_type>::Integrator(timestep)
            {}

            VelocityVerletIntegrator
            (
                double timestep,
                interaction_type interaction,
                boundary_condition_type boundary_condition
            )
                : Integrator<interaction_type, boundary_condition_type>::Integrator(
                    timestep, interaction, boundary_condition
                )
            {}

            // Evolves time by one step
            virtual SystemState& operator() (SystemState& state) override
        {
            /**
             * The Velocity Verlet algorithm splits the integration into two half-steps, with the
             * force calculation in between.
             */

            // First increment the velocities by half a time step:
            state.velocities += (1./2.) * state.forces * this->timestep_;

            // With the half-incremented velocities, give positions and displacements a full
            // increment:
            auto position_increment = state.velocities * this->timestep_;
            state.positions += position_increment;
            state.displacements += position_increment;

            // Need to impose boundary conditions here
            state | this->boundary_condition_;

            // Now with the new positions, compute the new forces
            state | this->interaction_;

            // Finally, with the new forces, increment the velocities by a second half-step:
            state.velocities += (1./2.) * state.forces * this->timestep_;

            return state;
        }
    };

    // Template argument deduction guide
    VelocityVerletIntegrator(double)
        -> VelocityVerletIntegrator<decltype(physics::identity_operator),
                                    decltype(physics::identity_operator)>;
} // namespace engine

#endif
