/**
 * pairwise_force.hpp
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

#ifndef LJ_PAIRWISE_FORCE_HPP
#define LJ_PAIRWISE_FORCE_HPP

#include <Eigen/Dense>

namespace physics
{
    struct ForceContribution
    {
        /**
         * A ForceContribution is the collection of data calculated by a PairwiseForce, described below.
         */

        Eigen::Vector4d force;
        double potential;
        double virial;
    };

    class PairwiseForce
    {
        /**
         * A PairwiseForce is a functor which implements pairwise forces between particles.
         * It accepts a vector
         * 
         *      r_{ij} = directed displacement to i from j = r_i - r_j
         * 
         * where r_i and r_j are the position vectors of particles i and j.  It then returns a
         * structure which contains
         * 
         *      F_{ij} = force on i due to j
         *      V_{ij} = contribution to the potential energy from i and j
         *      W_{ij} = contribution to the virial from i and j
         * 
         * The potential and virial contributions should be computed just once for each pair of
         * particles, which means the force contribution is also computed just once.  By Newton's
         * second law, one automatically gets
         * 
         *      F_{ji} = -F_{ij}
         * 
         * so this gives all the contributions to forces and energies from this pair of particles.
         */

        public:
            virtual ForceContribution operator() (Eigen::Vector4d) const = 0;
    };

    // Single instance of null force law which may be useful
    inline class : PairwiseForce
    {
        public:
            virtual ForceContribution operator()(Eigen::Vector4d rij [[maybe_unused]])
                const override
            {
                return ForceContribution{Eigen::Vector4d::Zero(), 0.0, 0.0};
            }
    } null_pairwise_force;
} // namespace physics
#endif
