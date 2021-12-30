/**
 * pairwise_force.cpp
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

#include <lennardjonesium/physics/pairwise_force.hpp>

namespace physics
{
    ZeroPairwiseForce::ZeroPairwiseForce(double cutoff_length)
        : cutoff_length_{cutoff_length}
    {}

    ForceContribution ZeroPairwiseForce::operator() (Eigen::Vector4d separation) const
    {
        return ForceContribution{{0, 0, 0, 0}, 0, 0};
    }

    double ZeroPairwiseForce::cutoff_length() const
    {return cutoff_length_;}
    
    double ZeroPairwiseForce::square_cutoff_length() const
    {return cutoff_length_ * cutoff_length_;}
} // namespace physics

