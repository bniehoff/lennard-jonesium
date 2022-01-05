/**
 * naive_particle_pair_filter.cpp
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

#include <array>
#include <ranges>

#include <Eigen/Dense>

#include <lennardjonesium/draft_cpp23/generator.hpp>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>
#include <lennardjonesium/engine/naive_particle_pair_filter.hpp>

namespace engine
{
    std::generator<ParticlePair>
    NaiveParticlePairFilter::operator() (const physics::SystemState& state) const
    {
        /**
         * Naively, we do a double loop over every pair of particles in the system, and return
         * the ones whose separation is close enough.
         * 
         * We also need to take into account the periodic boundary conditions.  In principle, given
         * a pair of particles (i, j), particle i might also have interactions with any number of
         * the periodic images of j.  The most extreme case is when the cutoff distance is equal to
         * the bounding box size, in which case it is possible for i to be close enough to j and 7
         * of its 26 images (for example, take i in the corner of the box, and j at the center).
         * 
         * The most naive thing to do is to always check all 26 images, but this is wasteful, as
         * at most 7 of them will be within the cutoff distance.  However, at the moment I cannot
         * think of a clever, simple method to check only the likely ones.  In any case, the
         * purpose of this class is to be as simple to code as possible, while not being very
         * efficient, to be used as a baseline for comparing to more clever solutions.
         */

        // We use these to iterate over the periodic images in a uniform way
        constexpr std::array<double, 4> images[27] = {
            {-1, -1, -1,  0},   {-1, -1,  0,  0},   {-1, -1,  1,  0},
            {-1,  0, -1,  0},   {-1,  0,  0,  0},   {-1,  0,  1,  0},
            {-1,  1, -1,  0},   {-1,  1,  0,  0},   {-1,  1,  1,  0},
            { 0, -1, -1,  0},   { 0, -1,  0,  0},   { 0, -1,  1,  0},
            { 0,  0, -1,  0},   { 0,  0,  0,  0},   { 0,  0,  1,  0},
            { 0,  1, -1,  0},   { 0,  1,  0,  0},   { 0,  1,  1,  0},
            { 1, -1, -1,  0},   { 1, -1,  0,  0},   { 1, -1,  1,  0},
            { 1,  0, -1,  0},   { 1,  0,  0,  0},   { 1,  0,  1,  0},
            { 1,  1, -1,  0},   { 1,  1,  0,  0},   { 1,  1,  1,  0}
        };

        for (int i : std::views::iota(0, state.particle_count()))
        {
            for (int j : std::views::iota(i + 1, state.particle_count()))
            {
                for (const auto& image : images)
                {
                    // This allows us to do Eigen arithmetic on the raw array
                    Eigen::Map<const Eigen::Array4d> image_array(image.data());

                    // Find the separation vector to i from the image of j
                    auto separation = state.positions.col(i) - state.positions.col(j) - (
                        image_array * bounding_box_.array()
                    ).matrix();

                    // Return if it is shorter than the cutoff distance, otherwise skip
                    if (separation.squaredNorm() < cutoff_distance_ * cutoff_distance_)
                    {
                        co_yield ParticlePair{separation, i, j};
                    }
                }
            }
        }
    }
} // namespace engine

