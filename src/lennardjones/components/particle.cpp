/**
 * particle.cpp
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

#include <Eigen/Dense>

#include <lennardjones/tools/aligned_vectors.hpp>
#include <lennardjones/components/particle.hpp>

namespace components {
    // Initialize the Particle ID counter
    int Particle::global_id_ = 0;

    // Constructors
    Particle::Particle(AlignedVector3d position,
                       AlignedVector3d velocity,
                       AlignedVector3d acceleration)
        : position_{position}, velocity_{velocity}, acceleration_{acceleration},
          displacement_{AlignedVector3d::Zero()}, id_{next_global_id_()}
    {}

    Particle::Particle()
        : Particle::Particle(AlignedVector3d::Zero(), AlignedVector3d::Zero(),
                             AlignedVector3d::Zero())
    {}
}
