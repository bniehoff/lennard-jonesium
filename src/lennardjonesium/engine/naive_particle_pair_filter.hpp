/**
 * naive_particle_pair_filter.hpp
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

#ifndef LJ_NAIVE_PARTICLE_PAIR_FILTER_HPP
#define LJ_NAIVE_PARTICLE_PAIR_FILTER_HPP

#include <lennardjonesium/draft_cpp23/generator.hpp>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>

namespace engine
{
    class NaiveParticlePairFilter : public ParticlePairFilter
    {
        public:
            using ParticlePairFilter::ParticlePairFilter;

            // Generate the ParticlePairs filtered by separation distance
            virtual std::generator<ParticlePair>
            operator() (const physics::SystemState&) const override;
    };
} // namespace engine


#endif
