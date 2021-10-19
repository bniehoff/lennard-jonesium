/**
 * system_state.cpp
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

#include <lennardjonesium/engine/system_state.hpp>

namespace engine {
    SystemState::SystemState(int particle_count)
        : kinetic_energy{0}, potential_energy{0}, virial{0}
    {
        set_particle_count(particle_count);
    }

    SystemState& SystemState::set_particle_count(int particle_count)
    {
        /**
         * Create initial SystemState with all entries set to zero.  The particle count is not
         * known until runtime, so we can't use a template class.
         */

        positions.setZero(4, particle_count);
        velocities.setZero(4, particle_count);
        displacements.setZero(4, particle_count);
        forces.setZero(4, particle_count);

        return *this;
    }

    SystemState& operator|
        (SystemState& state, std::function<SystemState& (SystemState&)> operation)
    {
        return operation(state);
    }
}
