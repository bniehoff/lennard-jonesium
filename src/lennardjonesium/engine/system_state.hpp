/**
 * system_state.hpp
 * 
 * Copyright (c) 2021 Benjamin E. Niehoff
 * 
 * This file is part of Lennard-Jones-Particles.
 * 
 * Lennard-Jones-Particles is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Lennard-Jones-Particles is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Lennard-Jones-Particles.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LJ_SYSTEM_STATE_HPP
#define LJ_SYSTEM_STATE_HPP

#include <Eigen/Dense>

using Eigen::Matrix4Xd;

namespace engine {
    struct SystemState {
        /**
         * We use 4xN matrices so that each of the columns will be aligned for vectorization.
         * 
         * Eigen names the components of a 4-vector .x(), .y(), .z(), .w(), in that order.  So,
         * we will leave the 4th component unused (it must be set to zero in order for arithmetic
         * with += to work nicely).
         */

        Matrix4Xd positions;        // Position within the bounding box
        Matrix4Xd velocities;       // Velocity
        Matrix4Xd accelerations;    // Force or acceleration, since mass is normalized to 1
        Matrix4Xd displacements;    // Total displacement ignoring boundary conditions

        /**
         * The argument specifies the size of the system, not any of the data in it, so we use
         * explicit to make sure this constructor can't be used for implicit conversions from int.
         */
        explicit SystemState(int particle_count = 0) { set_particle_count(particle_count); }

        /**
         * This both sets the particle count AND initializes all data to zero.
         */
        SystemState& set_particle_count(int particle_count);
    };
}

#endif
