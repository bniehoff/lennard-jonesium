/**
 * measurements.hpp
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

#ifndef LJ_MEASUREMENTS_HPP
#define LJ_MEASUREMENTS_HPP

#include <Eigen/Dense>

#include <lennardjonesium/physics/system_state.hpp>

namespace physics
{
    /**
     * We define some various types of measurements that can be made on the SystemState.
     * These are simply functions, which return a value of the appropriate type
     */

    double kinetic_energy(const SystemState&);

    Eigen::Vector4d total_momentum(const SystemState&);

    Eigen::Vector4d total_force(const SystemState&);

    Eigen::Vector4d center_of_mass(const SystemState&);

    Eigen::Vector4d total_angular_momentum
        (const SystemState& state, const Eigen::Ref<const Eigen::Vector4d>& center);
        
    Eigen::Vector4d total_angular_momentum(const SystemState& state);
    
    /**
     * The inertia tensor is given as a 4x4 matrix for alignement reasons.  The upper 3x3 block
     * is the 3-dimensional inertia tensor.  The (i, 4) and (4, j) elements are zero.  The (4, 4)
     * element is equal to 1/2 times the trace of the 3-dimensional inertia tensor, although its
     * actual value should never come into play, since 3d vectors are represented as 4d vectors
     * with a 0 as their 4th component.
     * 
     * Note also that the inertia tensor can have zero eigenvalues!  This happens only if all of
     * the particles are collinear with each other and with the center point.  In most practical
     * situations, this will not happen, but in general one must take care when inverting the
     * inertia tensor.
     */

    Eigen::Matrix4d inertia_tensor
        (const SystemState& state, const Eigen::Ref<const Eigen::Vector4d>& center);

    Eigen::Matrix4d inertia_tensor(const SystemState& state);
} // namespace physics

#endif
