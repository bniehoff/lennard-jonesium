/**
 * particle_pair_filter.cpp
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

#include <cassert>
#include <array>
#include <ranges>

#include <Eigen/Dense>

#include <lennardjonesium/tools/aligned_generator.hpp>
#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/tools/cell_list_array.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>

namespace engine
{
    bool operator== (const ParticlePair& a, const ParticlePair& b)
    {
        /**
         * The elements of a pair could have been given in the opposite order, in which case the
         * separation vector would be in the opposite direction.
         */
        return (
            (a.first == b.first && a.second == b.second && a.separation.isApprox(b.separation))
            or
            (a.first == b.second && a.second == b.first && a.separation.isApprox(-b.separation))
        );
    }

    ParticlePairFilter::ParticlePairFilter
        (const tools::BoundingBox& bounding_box, double cutoff_distance)
        : bounding_box_{bounding_box}, cutoff_distance_{cutoff_distance}
    {
        /**
         * We need to check that the bounding box is at least as large as the cutoff distance, or
         * else the force cannot be interpreted as short range (if the cutoff distance were larger
         * than the periodic boundary distance, then a particle would be able to exert forces on
         * itself, and this needs special treatment that is beyond the scope of this project.)
         */

        assert(
            (bounding_box.array().head<3>() >= cutoff_distance).all() &&
            "Simulation box size is less than the cutoff distance"
        );
    }

    tools::aligned_generator<ParticlePair>
    NaiveParticlePairFilter::operator() (const physics::SystemState& state)
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
                    auto separation = (
                        state.positions.col(i) - state.positions.col(j)
                        - (image_array * bounding_box_.array()).matrix()
                    );

                    // Return if it is shorter than the cutoff distance, otherwise skip
                    if (separation.squaredNorm() < cutoff_distance_ * cutoff_distance_)
                    {
                        co_yield ParticlePair{separation, i, j};
                    }
                }
            }
        }
    }

    CellListParticlePairFilter::CellListParticlePairFilter
        (const tools::BoundingBox& bounding_box, double cutoff_distance)
        : ParticlePairFilter::ParticlePairFilter{bounding_box, cutoff_distance},
          cell_list_array_{bounding_box, cutoff_distance}
    {}

    tools::aligned_generator<ParticlePair>
    CellListParticlePairFilter::operator() (const physics::SystemState& state)
    {
        /**
         * We use a CellListArray to find the pairs of particles which are within the cutoff
         * distance of each other.
         */

        // 1. Repopulate the CellListArray:
        // Start by clearing it.
        cell_list_array_.clear();

        // Next compute the cell indices of every particle.
        Eigen::Array4Xi cell_indices = (
            state.positions.array().colwise() *
            (cell_list_array_.shape().cast<double>() / bounding_box_.array())
        ).floor().cast<int>();

        // Then assign each particle to its corresponding cell
        for (int i : std::views::iota(0, cell_indices.cols()))
        {
            auto cell_index = cell_indices.col(i);
            cell_list_array_(cell_index[0], cell_index[1], cell_index[2]).push_back(i);
        }

        // 2. Find all the sufficiently-close particle pairs that lie within a single cell
        for (const auto& cell : cell_list_array_.cells())
        {
            for (int i : std::views::iota(0, static_cast<int>(cell.size())))
            {
                for (int j : std::views::iota(i + 1, static_cast<int>(cell.size())))
                {
                    auto separation = state.positions.col(cell[i]) - state.positions.col(cell[j]);

                    if (separation.squaredNorm() < cutoff_distance_ * cutoff_distance_)
                    {
                        co_yield ParticlePair{separation, cell[i], cell[j]};
                    }
                }
            }
        }

        // 3. Find the particle pairs from adjacent cells
        for (const auto& pair : cell_list_array_.adjacent_pairs())
        {
            for (int i : std::views::iota(0, static_cast<int>(pair.first.size())))
            {
                for (int j : std::views::iota(0, static_cast<int>(pair.second.size())))
                {
                    auto separation = (
                        state.positions.col(pair.first[i]) - state.positions.col(pair.second[j])
                        - (pair.lattice_image.cast<double>() * bounding_box_.array()).matrix()
                    );

                    if (separation.squaredNorm() < cutoff_distance_ * cutoff_distance_)
                    {
                        co_yield ParticlePair{separation, pair.first[i], pair.second[j]};
                    }
                }
            }
        }
    }
} // namespace engine

