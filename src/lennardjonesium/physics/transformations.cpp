/**
 * transformations.hpp
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
#include <cmath>
#include <ranges>

#include <Eigen/Dense>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/physics/transformations.hpp>

namespace physics
{
    SystemState::Operator set_momentum(const Eigen::Ref<const Eigen::Vector4d>& momentum)
    {
        return [momentum](SystemState& state) -> SystemState&
        {
            assert(state.particle_count() > 0 && "Cannot set momentum of empty state");

            /**
             * Take the difference in momenta divided by the total mass to get the velocity delta
             * which should be added to every velocity in the system
             */

            state.velocities.colwise() += (
                (momentum - total_momentum(state)) / static_cast<double>(state.particle_count())
            );

            return state;
        };
    }

    SystemState::Operator set_angular_momentum(
        const Eigen::Ref<const Eigen::Vector4d>& angular_momentum,
        const Eigen::Ref<const Eigen::Vector4d>& center
    )
    {
        return [angular_momentum, center](SystemState& state) -> SystemState&
        {
            assert(state.particle_count() > 0 && "Cannot set angular momentum of empty state");

            /**
             * To change the angular momentum, we must compute the necessary change in angular
             * velocity.  This means solving
             * 
             *      (L' - L) = (inertia_tensor) * (omega' - omega)
             * 
             * for (omega' - omega).  We use a full-pivot Householder linear solver from Eigen.
             */

            Eigen::Vector4d delta_omega = inertia_tensor(state, center).fullPivHouseholderQr()
                .solve(angular_momentum - total_angular_momentum(state, center));

            /**
             * Given the change in angular velocity, we must then apply it to the system velocities
             * using the formula
             * 
             *      (velocity' - velocity) = (omega' - omega) x (position - center)
             * 
             * where x is the cross product.  Unfortunately we have to write this loop explicitly,
             * since Eigen does not provide a colwise() version of cross3().
             */

            for (int i : std::views::iota(0, state.particle_count()))
            {
                state.velocities.col(i) += delta_omega.cross3(state.positions.col(i) - center);
            }

            return state;
        };
    }

    SystemState::Operator set_temperature(double temperature)
    {
        return [temperature](SystemState& state) -> SystemState&
        {
            /**
             * To change the temperature of the state, we note that temperature scales quadratically
             * with velocity
             * 
             *      T = (1/3) sum(velocity * velocity)
             * 
             * so we can correct the temperature by scaling the velocities by the square root of
             * the ratio of temperatures.
             */

            assert(
                physics::temperature(state) > 0
                && "Cannot scale temperature of zero-temperature state"
            );

            state.velocities *= std::sqrt(temperature / physics::temperature(state));

            return state;
        };
    }
} // namespace physics
