/**
 * particle.hpp
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

#ifndef LJ_PARTICLE_HPP
#define LJ_PARTICLE_HPP

#include <lennardjones/tools/aligned_vectors.hpp>

namespace components
{
    class Particle
    {
        private:
            AlignedVector3d position_;      // position with boundary conditions applied
            AlignedVector3d velocity_;      // velocity
            AlignedVector3d acceleration_;  // force or acceleration (mass normalized to 1)
            AlignedVector3d displacement_;  // total displacement ignoring boundary conditions
        
            int id_;                        // index of this particle in master list

            static int global_id_;
            static int next_global_id_() { return global_id_++; }
        
        public:
            // Constructors
            Particle(AlignedVector3d position, AlignedVector3d velocity);

            Particle() : Particle(AlignedVector3d::Zero(), AlignedVector3d::Zero())
            {}

            Particle(const Particle &);

            Particle & operator=(const Particle &);

            // Getters
            const AlignedVector3d & position()     const { return position_;     }
            const AlignedVector3d & velocity()     const { return velocity_;     }
            const AlignedVector3d & acceleration() const { return acceleration_; }
            const AlignedVector3d & displacement() const { return displacement_; }

            int id() const { return id_; }

            /**
             * TODO: Need some way to increment the position and automatically stay within
             * boundary, if given.  Also think how this might interact with force calculation?
             */

            // Manipulate the global ID count if needed
            static void set_global_id(int id) { global_id_ = id; }
            static void reset_global_id() { set_global_id(0); }
    };
}

#endif
