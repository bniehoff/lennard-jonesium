/**
 * operator.hpp
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

#ifndef LJ_OPERATOR_HPP
#define LJ_OPERATOR_HPP

#include <functional>
#include <type_traits>

#include <lennardjonesium/physics/system_state.hpp>

using physics::SystemState;

namespace physics
{
    /**
     * Define the concept of an Operator that acts on the SystemState.
     * 
     * This definition is more generic than necessary, we will only use the SystemState version.
     */
    template <typename Op, typename S = SystemState>
    concept Operator = std::invocable<Op, S&> and std::is_invocable_r_v<S&, Op, S&>;

    /**
     * Operator that simply returns the state without change
     */
    SystemState& identity_operator(SystemState&);

    /**
     * The following are template functions, so their definitions must be in the header rather than
     * in the .cpp file.
     */

    /**
     * Operators can act on SystemStates via the pipe syntax
     * 
     *      state | op1 | op2 | ...;
     */
    SystemState& operator| (SystemState& s, Operator auto op)
    {
        return op(s);
    }

    /**
     * Operators together in a pipeline can also be combined into a single operator
     * 
     *      combined_op = op1 | op2 | op3 | ...;
     */
    Operator auto operator| (Operator auto op1, Operator auto op2)
    {
        return [op1=std::move(op1), op2=std::move(op2)](SystemState& s) -> SystemState&
            {
                return op2(op1(s));
            };
    }
} // namespace physics

#endif
