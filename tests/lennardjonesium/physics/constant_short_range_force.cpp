/**
 * constant_short_range_force.cpp
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

#include <src/lennardjonesium/physics/forces.hpp>

#include <tests/lennardjonesium/physics/constant_short_range_force.hpp>

ConstantShortRangeForce::ConstantShortRangeForce(double force, double cutoff_length)
    : force_{force}, cutoff_distance_{cutoff_length}
{}

physics::ForceContribution ConstantShortRangeForce::operator()
    (const Eigen::Ref<const Eigen::Vector4d>& separation) const
{
    // We will use the norm of the separation vector
    double norm = separation.norm();

    physics::ForceContribution force_contribution;

    // The result depends on whether we are inside the cutoff distance or not
    if (norm < cutoff_distance_)
    {
        // The potential grows linearly with distance:
        force_contribution.potential = force_ * (norm - cutoff_distance_);

        // The virial is -r times the derivative of the potential with respect to r:
        force_contribution.virial = -force_ * norm;

        // The force is the virial times (/vec r)/r^2, which should be constant magnitude:
        force_contribution.force = force_contribution.virial * separation / (norm * norm);
    }
    else
    {
        // All contributions are zero
        force_contribution.potential = 0;
        force_contribution.virial = 0;
        force_contribution.force = Eigen::Vector4d::Zero();
    }

    return force_contribution;
}

double ConstantShortRangeForce::cutoff_distance() const
{return cutoff_distance_;}
