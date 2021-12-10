/**
 * dynamics.hpp
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

#ifndef LJ_DYNAMICS_HPP
#define LJ_DYNAMICS_HPP

#include <Eigen/Dense>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/pairwise_force.hpp>
#include <lennardjonesium/tools/dimensions.hpp>

namespace engine
{
    class Dynamics
    {
        /**
         * A Dynamics object calculates the forces on the particles in the system, and imposes
         * boundary conditions.  It is essentially responsible for preparing the system for the
         * next time step of integration.
         * 
         * We will assume periodic boundary conditions and short-range forces which have some
         * cutoff scale.  Treatment of long-range forces in an infinite (or periodic) system
         * requires special techniques (such as Ewald summation), which are not needed for the
         * Lennard-Jones potential.
         * 
         * We also assume that forces depend only on positions, and not velocities (so, magnetic
         * forces are excluded, but they are long-range anyway).
         * 
         * Obviously one can imagine more general types of boundary conditions and forces, but
         * implementing those (or leaving open the flexibility to implement them) is beyond the
         * scope of this project.
         */

        public:
            // Imposes the boundary conditions and calculates the forces of the system
            virtual physics::SystemState& operator() (physics::SystemState&) const = 0;

            // Construct the system's Dynamics from a bounding box and pairwise force
            Dynamics(const tools::Dimensions&, const physics::PairwiseForce&);

            // Construct a trivial Dynamics that only imposes boundary conditions, and has no forces
            Dynamics(const tools::Dimensions&);

        protected:
            /**
             * We store the bounding box internally as an array type so that it can more easily
             * be used in Eigen broadcasting expressions.
             */
            const Eigen::Array4d dimensions_;

            /**
             * Internally, we use a pointer to the PairwiseForce, which may be null in the case
             * that there are no forces.  (There is no sense in which a zero force can be thought
             * of as short-range, so we cannot formulate the lack of forces as some sort of
             * "default pairwise force".)
             */
            const physics::PairwiseForce *const force_;

            // Internal constructor that directly assigns the member variables
            Dynamics(const tools::Dimensions&, const physics::PairwiseForce *const);
    };
} // namespace engine

#endif