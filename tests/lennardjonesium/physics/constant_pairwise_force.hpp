/**
 * constant_pairwise_force.hpp
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

#ifndef LJ_CONSTANT_PAIRWISE_FORCE_HPP
#define LJ_CONSTANT_PAIRWISE_FORCE_HPP

#include <src/lennardjonesium/physics/pairwise_force.hpp>

// We define this derived class for testing purposes only
class ConstantPairwiseForce : public physics::PairwiseForce
{
    public:
        ConstantPairwiseForce(double force, double cutoff_length);

        virtual physics::ForceContribution operator() (Eigen::Vector4d) const override;
        virtual double cutoff_length() const override;
        virtual double square_cutoff_length() const override;
    
    protected:
        const double force_;
        const double cutoff_length_;
};

#endif
