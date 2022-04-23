/**
 * constant_short_range_force.cpp
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

#include <Eigen/Dense>

#include <src/cpp/lennardjonesium/physics/forces.hpp>

#include <tests/cpp/mock/constant_short_range_force.hpp>

namespace mock
{
    physics::ForceContribution ConstantShortRangeForce::compute
        (const Eigen::Ref<const Eigen::Vector4d>& separation) const
    {
        // We will use the norm of the separation vector
        double norm = separation.norm();

        // The result depends on whether we are inside the cutoff distance or not
        if (norm < parameters_.cutoff_distance)
        {
            /**
             * The potential changes linearly with distance.  The potential well shape is \/ for an
             * attractive (negative) force, or /\ for a repulsive (positive) force.
             */
            double potential = parameters_.strength * (parameters_.cutoff_distance - norm);

            // The virial is -r times the derivative of the potential with respect to r:
            double virial = parameters_.strength * norm;

            // The force is the virial times (\vec r)/r^2, which should be constant magnitude:
            auto force = virial * separation / (norm * norm);

            return {force, potential, virial};
        }
        else
        {
            // All contributions are zero
            return {Eigen::Vector4d::Zero(), 0.0, 0.0};
        }
    }
} // namespace mock