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

        // A Measurement observes the SystemState without modifying it
        using Measurement = std::function<const SystemState& (const SystemState&)>;

        // A Property gets a value from the SystemState
        template<typename T> requires (!std::convertible_to<std::remove_cvref<T>, SystemState>)
        using Property = std::function<T (const SystemState&)>;

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

        // It is also useful to keep track of the total time elapsed from the beginning
        double time{0.0};

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
     * We define a few basic Properties.  These just give a uniform syntax for accessing information
     * about the SystemState.
     */

    inline double potential_energy(const SystemState& state) {return state.potential_energy;}
    inline double virial(const SystemState& state) {return state.virial;}
    inline double time(const SystemState& state) {return state.time;}
    inline int particle_count(const SystemState& state) {return state.particle_count();}

    /**
     * It is useful to define concepts that refer to Operators, Measurements, and Properties.
     */
    template <class Op>
    concept Operator = std::invocable<Op, SystemState&>
        and std::is_invocable_r_v<SystemState&, Op, SystemState&>;

    template <class Mes>
    concept Measurement = std::invocable<Mes, const SystemState&>
        and std::is_invocable_r_v<const SystemState&, Mes, const SystemState&>;
    
    template <class Pr>
    concept Property = std::invocable<Pr, const SystemState&>
        and std::is_invocable_v<Pr, const SystemState&>
        and !Measurement<Pr> and !Operator<Pr>;

    /**
     * Operators can act on SystemStates via the pipe syntax
     * 
     *      state | op1 | op2 | ...;
     */
    SystemState& operator| (SystemState& s, const Operator auto& op) {return op(s);}

    /**
     * Similarly for Measurements
     * 
     *      state | me1 | me2 | ...;
     */
    const SystemState& operator| (const SystemState& s, const Measurement auto& me) {return me(s);}

    /**
     * Properties can only appear at the end of a chain
     * 
     *      state | ... | pr;
     */
    auto operator| (const SystemState& s, const Property auto& pr) {return pr(s);}

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

    /**
     * Measurements can also be combined
     * 
     *      combined_me = me1 | me2 | me3 | ...;
     */
    Measurement auto operator| (const Measurement auto& me1, const Measurement auto& me2)
    {
        using S = SystemState;
        return [me1=std::move(me1), me2=std::move(me2)](const S& s) -> const S&
            {return me2(me1(s));};
    }

    /**
     * An Operator followed by a Measurement is an Operator
     * 
     *      combined_op = op1 | me2 | me3 | ...;
     */
    Operator auto operator| (const Operator auto& op1, const Measurement auto& me2)
    {
        using S = SystemState;
        return [op1=std::move(op1), me2=std::move(me2)](S& s) -> S&
            {return const_cast<S&>(me2(op1(s)));};
    }

    /**
     * An Operator followed by a Property cannot be reduced, because the resulting object would not
     * be a function accepting a const reference.
     */

    /**
     * A Measurement followed by a Property is a Property
     * 
     *      combined_pr = me1 | pr2;
     */
    Property auto operator| (const Measurement auto& me1, const Property auto& pr2)
    {
        using S = SystemState;
        return [me1=std::move(me1), pr2=std::move(pr2)](const S& s) -> const S&
            {return pr2(me1(s));};
    }

} // namespace physics

#endif
