/**
 * system_state.hpp
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

#ifndef LJ_SYSTEM_STATE_HPP
#define LJ_SYSTEM_STATE_HPP

#include <utility>
#include <concepts>
#include <type_traits>
#include <functional>

#include <Eigen/Dense>

namespace physics
{
    struct SystemState
    {
        /**
         * SystemState will contain all the information about the particles in the system:
         * 
         *  positions
         *  velocities
         *  forces/accelerations
         *  total displacements (disregarding boundary conditions)
         *  *mean square displacement
         *  *kinetic energy
         *  potential energy
         *  virial
         *  (*kinetic energy tensor)
         *  (virial tensor)
         * 
         * The last two are not necessary for the most basic simulator, but may be interesting in
         * the future for measuring things like shear stress and pressure.
         * 
         * Note also that kinetic energy and mean square displacement are derived quantities, and
         * will be considered measurements rather than state variables (especially considering we
         * might not want to compute them every time step).
         * 
         * SystemState is merely a container for this data, and does not have any methods to
         * modify it.  Instead, the SystemState is acted on by Operators which modify the state.
         * 
         * (Mostly the reason for this is to learn how to use Concepts to define some generic
         * operator overloading; however, the unusual syntax for dealing with the SystemState also
         * helps make those lines stand out elsewhere in the code.)
         */

        // An Operator is a function that acts on the SystemState
        using Operator = std::function<SystemState& (SystemState&)>;

        /**
         * We use 4xN matrices so that each of the columns will be aligned for vectorization.
         * 
         * Eigen names the components of a 4-vector .x(), .y(), .z(), .w(), in that order.  So,
         * we will leave the 4th component unused (it must be set to zero in order for arithmetic
         * with += to work nicely).
         */

        // Kinematic quantities (properties of the motion itself)
        Eigen::Matrix4Xd positions;        // Position within the bounding box
        Eigen::Matrix4Xd velocities;       // Velocity
        Eigen::Matrix4Xd displacements;    // Total displacement ignoring boundary conditions

        // Dynamic quantities (arising from the interactions between particles)
        Eigen::Matrix4Xd forces;           // Force or acceleration, since mass is normalized to 1
        double potential_energy{0.0};      // Potential energy from particle interactions
        double virial{0.0};                // Virial from pairwise forces

        // Construct a SystemState with a given particle count
        explicit SystemState(int particle_count = 0);

        // Get the particle count
        int particle_count() const {return positions.cols();};
    };

    /**
     * Operator that simply returns the state without change.
     */
    inline auto identity_operator = [](SystemState& s) -> SystemState& {return s;};

    /**
     * Some additional useful Operators which modify the SystemState.
     */

    // Clears the force, potential energy, and virial, so that they can be recomputed
    SystemState& clear_dynamics(SystemState&);

    // Clears the displacements so that the main experiment can start from the current positions
    SystemState& clear_displacements(SystemState&);

    /**
     * The Operator concept allows us to define some syntax for functions that act on the
     * SystemState.
     */
    template <typename Op, typename S = SystemState>
    concept Operator = std::invocable<Op, S&> and std::is_invocable_r_v<S&, Op, S&>;

    /**
     * Operators can act on SystemStates via the pipe syntax
     * 
     *      state | op1 | op2 | ...;
     */
    SystemState& operator| (SystemState& s, const Operator auto& op) {return op(s);}

    /**
     * Operators together in a pipeline can also be combined into a single operator
     * 
     *      combined_op = op1 | op2 | op3 | ...;
     */
    Operator auto operator| (const Operator auto& op1, const Operator auto& op2)
    {
        using S = SystemState;
        return [op1=std::move(op1), op2=std::move(op2)](S& s) -> S& {return op2(op1(s));};
    }
} // namespace physics

#endif
