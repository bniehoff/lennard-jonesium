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

#include <lennardjonesium/engine/force_calculation.hpp>
#include <lennardjonesium/engine/boundary_condition.hpp>
#include <lennardjonesium/engine/integrator.hpp>

namespace engine
{
    // Create an integrator with the given time step, force_calculation, and boundary condition
    Integrator::Integrator(
        double timestep,
        const ForceCalculation& force_calculation,
        const BoundaryCondition& boundary_condition
    )
        : timestep_(timestep),
          force_calculation_(force_calculation),
          boundary_condition_(boundary_condition)
    {}

    // Delegate to the above constructor with null force calculation and boundary condition
    Integrator::Integrator(double timestep)
        : Integrator::Integrator(
            timestep, engine::null_force_calculation, engine::null_boundary_condition
        )
    {}
} // namespace engine
