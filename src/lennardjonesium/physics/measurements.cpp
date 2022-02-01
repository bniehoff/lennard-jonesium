/**
 * measurements.cpp
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

#include <ranges>

#include <Eigen/Dense>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/measurements.hpp>

namespace physics
{
    /**
     * We define some various types of measurements that can be made on the SystemState.
     * These are simply functions, which return a value of the appropriate type
     */

    double kinetic_energy(const SystemState& state)
    {
        return (1./2.) * state.velocities.colwise().squaredNorm().sum();
    }

    double mean_square_displacement(const SystemState& state)
    {
        assert(state.particle_count() > 0
            && "Cannot compute mean square displacement of empty state.");

        return (
            state.displacements.colwise().squaredNorm().sum()
            / static_cast<double>(state.particle_count())
        );
    }

    Eigen::Vector4d total_momentum(const SystemState& state)
    {
        return state.velocities.rowwise().sum();
    }

    Eigen::Vector4d total_force(const SystemState& state)
    {
        return state.forces.rowwise().sum();
    }

    Eigen::Vector4d center_of_mass(const SystemState& state)
    {
        return state.positions.rowwise().sum() / static_cast<double>(state.particle_count());
    }

    Eigen::Vector4d total_angular_momentum
        (const SystemState& state, const Eigen::Ref<const Eigen::Vector4d>& center)
    {
        // Unfortunately we have to write the loop explicitly
        Eigen::Vector4d angular_momentum = Eigen::Vector4d::Zero();

        for (int i : std::views::iota(0, state.particle_count()))
        {
            auto r = state.positions.col(i) - center;
            angular_momentum += r.cross3(state.velocities.col(i));
        }

        return angular_momentum;
    }

    Eigen::Matrix4d inertia_tensor
        (const SystemState& state, const Eigen::Ref<const Eigen::Vector4d>& center)
    {
        /**
         * To get the inertia tensor, we use the formula
         * 
         *      I_{ij} = |r|^2 \delta_{ij} - r_i r_j
         */

        // We also need to write this loop explicitly
        Eigen::Matrix4d inertia_tensor = Eigen::Matrix4d::Zero();

        for (auto position : state.positions.colwise())
        {
            auto r = position - center;
            inertia_tensor += r.squaredNorm() * Eigen::Matrix4d::Identity() - r * r.transpose();
        }

        return inertia_tensor;
    }
} // namespace physics
