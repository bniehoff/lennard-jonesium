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

#include <Eigen/Dense>

#include <lennardjones/tools/AlignedVector3d.hpp>

namespace components
{
    class Particle
    {
        /**
         * TODO: Should Particle enforce any invariants on these data, or let them be completely
         *       handled elsewhere?  We'd like to centralize the force/acceleration calculation in
         *       another class, but maybe Particle should do something to control how its data are
         *       updated.
         */
        public:
            AlignedVector3d position;
            AlignedVector3d displacement;
            AlignedVector3d velocity;
            AlignedVector3d acceleration;
        
        private:
            int id_;
            static int global_id_;
            static int next_global_id_() {return global_id_++;}
        
        public:
            // Constructor simply accepts the values given
            Particle(AlignedVector3d p, AlignedVector3d d, AlignedVector3d v, AlignedVector3d a)
                : position(p), displacement(d), velocity(v), acceleration(a), id_(next_global_id_())
            {}

            // Default constructor sets all to zero (delegate to standard constructor)
            Particle()
                : Particle{AlignedVector3d::Zero(), AlignedVector3d::Zero(),
                           AlignedVector3d::Zero(), AlignedVector3d::Zero()}
            {}

            // Getter for the ID number
            int id() const {return id_;}

            // Manipulate the global ID count if needed
            static void set_global_id(int id) {global_id_ = id;}
            static void reset_global_id() {set_global_id(0);}

            /**
             * Although particles have a unique ID and will be "moved" between Cells, they actually
             * need to be copied to a new memory location, so that they will continue to be local
             * in memory to the other particles in their new Cell.  So we define a copy constructor
             * and copy assignment
             */
            
            // Copy constructor
            Particle(const Particle & other)
                : position(other.position), displacement(other.displacement),
                  velocity(other.velocity), acceleration(other.acceleration),
                  id_(other.id_)
            {}

            // Copy assignment
            Particle & operator=(const Particle & other)
            {
                position = other.position;
                displacement = other.displacement;
                velocity = other.velocity;
                acceleration = other.acceleration;
                id_ = other.id_;

                return *this;
            }
    };

    // Initialize the Particle ID counter
    int Particle::global_id_ = 0;
}

#endif
