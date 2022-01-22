/**
 * aligned_generator.hpp
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

#ifndef LJ_ALIGNED_GENERATOR_HPP
#define LJ_ALIGNED_GENERATOR_HPP

#include <memory>

#include <Eigen/Dense>

#include <lennardjonesium/draft_cpp23/generator.hpp>

namespace tools
{
    /**
     * We define an aligned version of std::generator that shouldn't cause problems with fixed-size
     * Eigen types.
     */
    struct alignas(EIGEN_MAX_ALIGN_BYTES) eigen_align_t {};

    template<typename T, typename AlignmentType = eigen_align_t>
    using aligned_generator =
        std::generator<T, std::remove_cvref_t<T>, std::allocator<AlignmentType>>;
    
    // We also provide an alias for std::generator, so that we only need one include
    template<typename Ref, typename Val = std::remove_cvref_t<Ref>, proto_allocator Alloc = void>
    using generator = std::generator<Ref, Val, Alloc>;
} // namespace tools

#endif
