/**
 * velocity_verlet_integrator.cpp
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

#include <Eigen/Dense>

#include <lennardjonesium/engine/velocity_verlet_integrator.hpp>
#include <lennardjonesium/physics/system_state.hpp>

using physics::SystemState;

namespace engine
{
    SystemState& VelocityVerletIntegrator::operator() (SystemState& state)
    {
        /**
         * The Velocity Verlet algorithm splits the integration into two half-steps, with the
         * force calculation in between.
         */

        // First increment the velocities by half a time step:
        state.velocities += (1./2.) * state.forces * timestep_;

        // With the half-incremented velocities, give positions and displacements a full increment:
        auto position_increment = state.velocities * timestep_;
        state.positions += position_increment;
        state.displacements += position_increment;

        // Need to impose boundary conditions here
        boundary_condition_(state);

        // Now with the new positions, compute the new forces
        interactions_(state);

        // Finally, with the new forces, increment the velocities by a second half-step:
        state.velocities += (1./2.) * state.forces * timestep_;

        return state;
    }
} // namespace engine
