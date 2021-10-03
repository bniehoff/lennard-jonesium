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

#include <eigen3/Eigen/Dense>

using Eigen::Vector3d;

namespace components
{
    class Particle
    {
        public:
            Vector3d position;
            Vector3d displacement;
            Vector3d velocity;
            Vector3d acceleration;
        
        private:
            // Here we assume we have less than 32768 particles
            int id_;
            static int global_id_;
            static int nextGlobalID() {return global_id_++;}
        
        public:
            // Constructor simply accepts the values given
            Particle(Vector3d p, Vector3d d, Vector3d v, Vector3d a)
                : position(p), displacement(d), velocity(v), acceleration(a), id_(nextGlobalID())
            {}

            // Default constructor sets all to zero (delegate to standard constructor)
            Particle()
                : Particle{Vector3d::Zero(), Vector3d::Zero(), Vector3d::Zero(), Vector3d::Zero()}
            {}

            // Getter for the ID number
            int getID() {return id_;}

            // Manipulate the global ID count if needed
            static void setGlobalID(int id) {global_id_ = id;}
            static void resetGlobalID() {setGlobalID(0);}

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
            
            // We will not define a move constructor or move assignment operator
    };

    // Initialize the Particle ID counter
    int Particle::global_id_ = 0;
}

#endif
