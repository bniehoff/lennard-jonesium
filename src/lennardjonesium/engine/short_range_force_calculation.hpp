/**
 * short_range_force_calculation.hpp
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

#ifndef LJ_SHORT_RANGE_FORCE_CALCULATION_HPP
#define LJ_SHORT_RANGE_FORCE_CALCULATION_HPP

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>
#include <lennardjonesium/engine/force_calculation.hpp>

namespace engine
{
    class ShortRangeForceCalculation : public ForceCalculation
    {
        public:
            ShortRangeForceCalculation(
                const physics::ShortRangeForce& short_range_force,
                engine::ParticlePairFilter& particle_pair_filter
            );

            // Compute the forces resulting from this interaction
            virtual physics::SystemState& operator() (physics::SystemState&) const override;
        
        private:
            const physics::ShortRangeForce& short_range_force_;

            // Can't be const, because some types use internal state
            engine::ParticlePairFilter& particle_pair_filter_;
    };
} // namespace engine


#endif