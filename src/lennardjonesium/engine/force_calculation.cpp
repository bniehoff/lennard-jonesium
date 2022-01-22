/**
 * force_calculation.cpp
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

#include <ranges>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>
#include <lennardjonesium/engine/force_calculation.hpp>

namespace engine
{
    ShortRangeForceCalculation::ShortRangeForceCalculation(
        const physics::ShortRangeForce& short_range_force,
        engine::ParticlePairFilter& particle_pair_filter
    )
        : short_range_force_{short_range_force}, particle_pair_filter_{particle_pair_filter}
    {}

    physics::SystemState&
    ShortRangeForceCalculation::operator() (physics::SystemState& state) const
    {
        /**
         * We need to get the ForceContribution from each pair of particles and add them to the
         * system state.  We use the particle pair filter to obtain pairs that are within the
         * cutoff distance of each other.
         */

        // First clear the dynamical quantities
        state.clear_dynamical_quantities();

        // Now iterate over the pairs of particles
        for (const auto& pair : particle_pair_filter_(state))
        {
            auto force_contribution = short_range_force_(pair.separation);

            state.forces.col(pair.first) += force_contribution.force;
            state.forces.col(pair.second) -= force_contribution.force;

            state.potential_energy += force_contribution.potential;
            state.virial += force_contribution.virial;
        }

        return state;
    }

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
        for (int i : std::views::iota(0, state.particle_count()))
        {
            auto force_contribution = background_force_(state.positions.col(i));

            state.forces.col(i) += force_contribution.force;
            state.potential_energy += force_contribution.potential;
            state.virial += force_contribution.virial;
        }

        return state;
    }
} // namespace engine
