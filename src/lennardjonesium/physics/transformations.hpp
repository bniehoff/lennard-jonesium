/**
 * transformations.hpp
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

#ifndef LJ_TRANSFORMATIONS_HPP
#define LJ_TRANSFORMATIONS_HPP

#include <Eigen/Dense>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/measurements.hpp>

namespace physics
{
    /**
     * Transformations are Operators that change the SystemState in some way, such as scaling the
     * temperature or adding some amount of momentum.  They are useful mostly when setting up the
     * simulation, to ensure that the randomly-generated velocities have some desired properties.
     */

    SystemState::Operator set_momentum(const Eigen::Ref<const Eigen::Vector4d>& momentum);

    // NOTE: When setting the angular momentum, it is possible that the linear momentum can change!
    SystemState::Operator set_angular_momentum(
        const Eigen::Ref<const Eigen::Vector4d>& angular_momentum,
        const Eigen::Ref<const Eigen::Vector4d>& center = Eigen::Vector4d::Zero()
    );

    SystemState::Operator set_temperature(double temperature);

    inline SystemState::Operator zero_momentum() {return set_momentum(Eigen::Vector4d::Zero());}
    
    inline SystemState::Operator zero_angular_momentum(
        const Eigen::Ref<const Eigen::Vector4d>& center = Eigen::Vector4d::Zero()
    )
        {return set_angular_momentum(Eigen::Vector4d::Zero(), center);}
} // namespace physics


#endif
