/**
 * particle_pair_filter.cpp
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

#include <Eigen/Dense>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>

namespace engine
{
    bool operator== (const ParticlePair& a, const ParticlePair& b)
    {
        /**
         * The elements of a pair could have been given in the opposite order, in which case the
         * separation vector would be in the opposite direction.
         */
        return (
            (a.first == b.first) && (a.second == b.second) && (a.separation.isApprox(b.separation))
            or
            (a.first == b.second) && (a.second == b.first) && (a.separation.isApprox(-b.separation))
        );
    }

    ParticlePairFilter::ParticlePairFilter
        (const tools::BoundingBox& bounding_box, double cutoff_distance)
        : bounding_box_{bounding_box}, cutoff_distance_{cutoff_distance}
    {
        /**
         * We need to check that the bounding box is at least as large as the cutoff distance, or
         * else the force cannot be interpreted as short range (if the cutoff distance were larger
         * than the periodic boundary distance, then a particle would be able to exert forces on
         * itself, and this needs special treatment that is beyond the scope of this project.)
         */

        assert((
            "Simulation box size is less than the cutoff distance",
            (bounding_box.array().head<3>() >= cutoff_distance).all()
        ));
    }
} // namespace engine

