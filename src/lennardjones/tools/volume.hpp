/**
 * volume.hpp
 * 
 * Copyright (c) 2021 Benjamin E. Niehoff
 * 
 * This file is part of Lennard-Jones-Particles.
 * 
 * Lennard-Jones-Particles is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Lennard-Jones-Particles is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Lennard-Jones-Particles.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LJ_VOLUME_HPP
#define LJ_VOLUME_HPP

#include <lennardjones/tools/aligned_vectors.hpp>

namespace tools {
    class Volume {
        /**
         * Represents a rectilinear volume in space, and answers questions about whether a given
         * point is inside the volume, and if not, where it falls outside.
         */
        private:
            AlignedVector3d origin_;        // The corner with lowest (x, y, z) coordinates
            AlignedVector3d dimensions_;    // The (x, y, z) dimensions (must be non-negative)
        
        public:
            /**
             * We construct a Volume in space from an origin and dimensions.
             */
            Volume(AlignedVector3d origin, AlignedVector3d dimensions);

            Volume() : Volume(AlignedVector3d::Zero(), AlignedVector3d::Zero())
            {}

            /**
             * Compares whether a given point is inside this volume.  Returns (0, 0, 0) if so.
             * If not, returns a vector of integers showing which direction(s) the given point
             * lies outside the volume.  For example, (1, 0, -1) indicates that the given point
             * lies in the positive x and negative z direction from this volume.
             * 
             * (We don't overload the <=> operator because the return type would be confusing)
             */
            AlignedVector3i compare(AlignedVector3d);
    };
}

#endif
