/**
 * system_state.cpp
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

#include <cassert>

#include <lennardjonesium/physics/system_state.hpp>

namespace physics
{
    SystemState::SystemState(int particle_count)
    {
        // Create initial state with all quantities set to zero

        positions.setZero(4, particle_count);
        velocities.setZero(4, particle_count);
        displacements.setZero(4, particle_count);
        forces.setZero(4, particle_count);
    }

    // Clears the force, potential energy, and virial, so that they can be recomputed
    SystemState& clear_dynamics(SystemState& state)
    {
        state.forces.setZero(4, state.particle_count());
        state.potential_energy = 0;
        state.virial = 0;

        return state;
    }

    // Clears the displacements so that the main experiment can start from the current positions
    SystemState& clear_displacements(SystemState& state)
    {
        state.displacements.setZero(4, state.particle_count());

        return state;
    }
} // namespace physics
