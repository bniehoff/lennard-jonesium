/**
 * forces.hpp
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

#ifndef LJ_FORCES_HPP
#define LJ_FORCES_HPP

#include <Eigen/Dense>

namespace physics
{
    struct ForceContribution
    {
        /**
         * ForceContribution packages together the data that are needed to update the SystemState
         * when the force on a particle is calculated.  This includes not just the force value
         * itself, but also contributions to the potential energy and virial (and their tensors,
         * if those are later added).
         * 
         * This is more convenient than using a std::tuple of these quantities, because using a
         * struct allows us to name the fields.
         */

        Eigen::Vector4d force;

        double potential;
        double virial;
    };

    class ShortRangeForce
    {
        /**
         * ShortRangeForce describes a pairwise interparticle force that has a fixed range of
         * interaction; i.e., a cutoff distance, beyond which the force will not be felt.
         * 
         * ShortRangeForce is responsible for computing the ForceContribution between two particles
         * given their separation vector
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
            // Compute a ForceContribution from a separation vector
            virtual ForceContribution
            compute(const Eigen::Ref<const Eigen::Vector4d>& separation) const = 0;

            // Get the cutoff distance
            virtual double cutoff_distance() const = 0;

            // Make sure dynamically allocated derived classes are properly destroyed
            virtual ~ShortRangeForce() = default;
    };

    class BackgroundForce
    {
        /**
         * BackgroundForce describes a force that acts on all particles in the system.  We only
         * allow forces which are a function of position and not velocity (otherwise some
         * integration methods will become invalid).  So magnetic forces are excluded.
         */

        public:
            virtual ForceContribution
            compute(const Eigen::Ref<const Eigen::Vector4d>& position) const = 0;

            // Make sure dynamically allocated derived classes are properly destroyed
            virtual ~BackgroundForce() = default;
    };
} // namespace physics


#endif
