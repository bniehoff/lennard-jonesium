/**
 * particle_pair_filter.hpp
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

#ifndef LJ_PARTICLE_PAIR_FILTER_HPP
#define LJ_PARTICLE_PAIR_FILTER_HPP

#include <Eigen/Dense>

#include <lennardjonesium/tools/aligned_generator.hpp>
#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/tools/cell_list_array.hpp>
#include <lennardjonesium/physics/system_state.hpp>

namespace engine
{
    struct ParticlePair
    {
        /**
         * ParticlePair is the collection of information needed to evaluate the ShortRangeForce on
         * a pair of particles.  It contains their indices into the SystemState arrays, as well as
         * the separation vector between them (which had to be calculated already, in order to
         * filter the particle pairs by their separation distance).  The separation vector already
         * takes into account any corrections due to wrapping around the periodic boundary
         * conditions.
         */

        Eigen::Vector4d separation;

        int first;
        int second;
    };

    // It is useful to be able to compare ParticlePairs
    bool operator== (const ParticlePair&, const ParticlePair&);

    class ParticlePairFilter
    {
        /**
         * ParticlePairFilter is responsible for finding all the pairs of particles which are
         * within the cutoff radius of each other, for the purpose of calculating short-range
         * forces.  For example, a naive approach would be to double loop over all pairs of
         * particles and forward those whose separation distance is less than the cutoff.  In our
         * case, we will use the more sophisticated technique of Cell Lists.
         */

        public:
            // This constructor should be inherited by derived classes with `using`
            ParticlePairFilter(tools::BoundingBox bounding_box, double cutoff_distance);

            // Generate the ParticlePairs filtered by separation distance
            virtual tools::aligned_generator<ParticlePair>
            pairs(const physics::SystemState&) = 0;

            // Make sure dynamically allocated derived classes are properly destroyed
            virtual ~ParticlePairFilter() = default;
        
        protected:
            const tools::BoundingBox bounding_box_;
            double cutoff_distance_;
    };

    // We define the following useful derived classes

    class NaiveParticlePairFilter : public ParticlePairFilter
    {
        public:
            using ParticlePairFilter::ParticlePairFilter;

            // Generate the ParticlePairs filtered by separation distance
            virtual tools::aligned_generator<ParticlePair>
            pairs(const physics::SystemState&) override;
    };

    class CellListParticlePairFilter : public ParticlePairFilter
    {
        public:
            CellListParticlePairFilter
                (tools::BoundingBox bounding_box, double cutoff_distance);
            
            // Generate the ParticlePairs filtered by separation distance
            virtual tools::aligned_generator<ParticlePair>
            pairs(const physics::SystemState&) override;
        
        protected:
            tools::CellListArray cell_list_array_;
    };
} // namespace engine


#endif
