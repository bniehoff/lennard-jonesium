/**
 * constant_short_range_force.hpp
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

#ifndef LJ_CONSTANT_SHORT_RANGE_FORCE_HPP
#define LJ_CONSTANT_SHORT_RANGE_FORCE_HPP

#include <src/lennardjonesium/physics/forces.hpp>

namespace mock
{
    // We define this derived class for testing purposes only
    class ConstantShortRangeForce : public physics::ShortRangeForce
    {
        public:
            /**
             * A negative force value is an attractive force, and a positive value is repulsive.
             */
            ConstantShortRangeForce(double force, double cutoff_length);

            virtual physics::ForceContribution
            operator() (const Eigen::Ref<const Eigen::Vector4d>& separation) const override;

            virtual double cutoff_distance() const override;
        
        protected:
            const double force_;
            const double cutoff_distance_;
    };
} // namespace mock

#endif
