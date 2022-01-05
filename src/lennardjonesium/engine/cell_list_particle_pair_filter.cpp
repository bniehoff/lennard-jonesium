/**
 * cell_list_particle_pair_filter.cpp
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

#include <Eigen/Dense>

#include <lennardjonesium/draft_cpp23/generator.hpp>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/tools/cell_list_array.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>
#include <lennardjonesium/engine/cell_list_particle_pair_filter.hpp>

namespace engine
{
    CellListParticlePairFilter::CellListParticlePairFilter
        (const tools::BoundingBox& bounding_box, double cutoff_distance)
        : ParticlePairFilter::ParticlePairFilter{bounding_box, cutoff_distance},
          cell_list_array_{bounding_box, cutoff_distance}
    {}

    std::generator<ParticlePair>
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
        for (auto i : std::views::iota(0, cell_indices.cols()))
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
        for (auto pair : cell_list_array_.adjacent_pairs())
        {
            for (int i : std::views::iota(0, static_cast<int>(pair.first.size())))
            {
                for (int j : std::views::iota(0, static_cast<int>(pair.second.size())))
                {
                    auto separation = (
                        state.positions.col(pair.first[i]) - state.positions.col(pair.second[j]) -
                        (pair.lattice_image.cast<double>() * bounding_box_.array()).matrix()
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

