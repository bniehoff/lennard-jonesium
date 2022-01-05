/**
 * velocity_verlet_integrator.cpp
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

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/velocity_verlet_integrator.hpp>

namespace engine
{
    // Evolves time by one step
    physics::SystemState& VelocityVerletIntegrator::operator() (physics::SystemState& state) const
    {
        /**
         * The Velocity Verlet algorithm splits the integration into two half-steps, with the
         * force calculation in between.
         */

        // First get the half increment to the velocities using the current value of the forces
        auto velocity_half_step = state.velocities + (1./2.) * state.forces * timestep_;

        // With the half-incremented velocities, give positions and displacements a full
        // increment:
        auto position_increment = velocity_half_step * timestep_;
        state.positions += position_increment;
        state.displacements += position_increment;

        // Next impose boundary conditions and calculate forces
        state | boundary_condition_ | force_calculation_;

        // Finally, update the velocities using the first half increment and a second half
        // increment based on the new forces:
        state.velocities = velocity_half_step + (1./2.) * state.forces * timestep_;

        return state;
    }
} // namespace engine
