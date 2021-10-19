/**
 * interaction.hpp
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

#ifndef LJ_INTERACTION_HPP
#define LJ_INTERACTION_HPP

#include <eigen3/Eigen/Dense>

using Eigen::Vector4d;

namespace engine {
    /**
     * These need to be calculated at the same time.  Use structured bindings to access
     * the separate parts more conveniently.
     */
    struct ForceContribution {
        Vector4d force;
        double potential_energy;
        double virial;
    };
    
    class Interaction {
        /**
         * An Interaction defines a pairwise force that exists between particles.  It must provide
         * a method force_law() which takes a single vector (the difference between two particle
         * positions) and returns a structure containing the contributions to the force, potential
         * energy, and virial that arise from this interaction between the two particles separated
         * along the given vector.
         * 
         * If the interaction is short-ranged, then it should also provide a cutoff distance, which
         * can be used to optimize the implementation of pairwise calculations.
         */
        public:
            /**
             * The square of the cutoff distance (note that we work in square distances only, to
             * avoid having to take a square root).
             * 
             * If the value is <= 0, then we interpret this as a long-range force.
             */
            const double square_cutoff_distance;
        
            // By default we assume a long-range force; any negative number will do
            explicit Interaction(double square_cutoff_distance = -1.0)
                : square_cutoff_distance(square_cutoff_distance)
            {}

            /**
             * Computes the contributions to force, potential energy, and virial arising from two
             * particles along a given separation vector.
             */
            virtual ForceContribution force_law(Vector4d separation) = 0;
    };
}

#endif