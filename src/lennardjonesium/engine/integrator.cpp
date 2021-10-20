/**
 * integrator.cpp
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

#include <Eigen/Dense>

#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/engine/system_state.hpp>

using Eigen::Vector4d;

namespace engine {
    Integrator::Integrator(
        double timestep,
        SystemState::Operator interactions,
        SystemState::Operator boundary_conditions
    )
        : timestep_{timestep},
          interactions_{interactions},
          boundary_conditions_{boundary_conditions}
    {}
}
