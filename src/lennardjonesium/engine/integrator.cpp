/**
 * integrator.cpp
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

#include <utility>
#include <memory>
#include <ranges>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/force_calculation.hpp>
#include <lennardjonesium/engine/boundary_condition.hpp>
#include <lennardjonesium/engine/integrator.hpp>

namespace engine
{
    // Create an integrator with the given time step, force_calculation, and boundary condition
    Integrator::Integrator(
        double time_delta,
        std::unique_ptr<const BoundaryCondition> boundary_condition,
        std::unique_ptr<const ForceCalculation> force_calculation
    )
        : time_delta_{time_delta},
          boundary_condition_{std::move(boundary_condition)},
          force_calculation_{std::move(force_calculation)}
    {}

    // Delegate to the above constructor with null force calculation and boundary condition
    Integrator::Integrator(double time_delta)
        : Integrator::Integrator(time_delta, nullptr, nullptr)
    {}

    // Returns an Operator which integrates by count time steps
    physics::SystemState::Operator Integrator::operator() (int steps) const
    {
        using S = physics::SystemState;
        return [this, steps](S& s) -> S&
        {
            for ([[maybe_unused]] int i : std::views::iota(0, steps)) {s | *this;}
            return s;
        };
    }

    // Evolves time by one step
    physics::SystemState& VelocityVerletIntegrator::operator() (physics::SystemState& state) const
    {
        /**
         * The Velocity Verlet algorithm splits the integration into two half-steps, with the
         * force calculation in between.
         */

        // First, half-increment the velocities with the current forces
        state.velocities += (1./2.) * state.forces * time_delta_;

        // Using half-incremented velocities, increment positions by full time step
        auto position_increment = state.velocities * time_delta_;
        state.positions += position_increment;
        state.displacements += position_increment;

        // Next impose boundary conditions and calculate updated forces
        if (boundary_condition_) [[likely]] {state | *boundary_condition_;}
        if (force_calculation_) [[likely]] {state | *force_calculation_;}

        // Do the second half-increment with the updated forces
        state.velocities += (1./2.) * state.forces * time_delta_;

        // Update the elapsed time
        state.time += time_delta_;

        return state;
    }
} // namespace engine
