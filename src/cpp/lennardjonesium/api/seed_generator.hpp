/**
 * random_seed.hpp
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

#ifndef LJ_SEED_GENERATOR_HPP
#define LJ_SEED_GENERATOR_HPP

#include <random>

#include <lennardjonesium/engine/initial_condition.hpp>

namespace api
{
    class SeedGenerator
    {
        /**
         * SeedGenerator is thin wrapper around std::random_device for providing random seeds in a
         * way that is easy to communicate to Cython.  (On the Python side, one could also use
         * other sources of random seeds, such as Numpy).
         */
        public:
            inline std::random_device::result_type get() {return rd();}

            inline static constexpr std::random_device::result_type default_seed =
                engine::InitialCondition::random_number_engine_type::default_seed;

        private:
            std::random_device rd{};
    };
} // namespace api


#endif
