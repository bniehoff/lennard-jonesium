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

#include <lennardjonesium/engine/interaction.hpp>
#include <lennardjonesium/engine/boundary_condition.hpp>
#include <lennardjonesium/engine/integrator.hpp>

namespace engine
{
    // Generic constructor allows nullptr entries
    Integrator::Integrator(
        double timestep,
        const Interaction* interaction,
        const BoundaryCondition* boundary_condition
    )
        : timestep_(timestep),
          interaction_(interaction),
          boundary_condition_(boundary_condition)
    {}

    // Create an integrator with the given time step, interaction, and boundary condition
    Integrator::Integrator(
        double timestep,
        Interaction& interaction,
        BoundaryCondition& boundary_condition
    )
        : Integrator::Integrator(timestep, &interaction, &boundary_condition)
    {}

    // Delegate to the above constructor with null interaction and boundary condition
    Integrator::Integrator(double timestep)
        : Integrator::Integrator(timestep, nullptr, nullptr)
    {}

    void Integrator::set_boundary_condition(BoundaryCondition& boundary_condition)
    {
        boundary_condition_ = &boundary_condition;
    }
} // namespace engine
