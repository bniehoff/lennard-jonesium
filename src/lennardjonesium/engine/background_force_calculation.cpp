/**
 * background_force_calculation.cpp
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

#include <ranges>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/engine/background_force_calculation.hpp>

namespace engine
{
    BackgroundForceCalculation::BackgroundForceCalculation
        (const physics::BackgroundForce& background_force)
        : background_force_{background_force}
    {}

    physics::SystemState&
    BackgroundForceCalculation::operator() (physics::SystemState& state) const
    {
        /**
         * We need to get the ForceContribution from each particle and add it to the system state.
         */

        // First clear the dynamical quantities
        state.clear_dynamical_quantities();

        // Now iterate over the particles
        for (auto i : std::views::iota(0, state.particle_count()))
        {
            auto force_contribution = background_force_(state.positions.col(i));

            state.forces.col(i) += force_contribution.force;
            state.potential_energy += force_contribution.potential;
            state.virial += force_contribution.virial;
        }

        return state;
    }
} // namespace engine

