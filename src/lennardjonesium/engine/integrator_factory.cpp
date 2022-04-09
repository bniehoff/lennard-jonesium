/**
 * integrator_factory.cpp
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

#include <exception>
#include <memory>
#include <utility>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/engine/boundary_condition.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>
#include <lennardjonesium/engine/force_calculation.hpp>
#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/engine/integrator_factory.hpp>

namespace engine
{
    std::unique_ptr<const Integrator> IntegratorFactory::make_integrator(
        double time_delta,
        tools::BoundingBox bounding_box,
        std::unique_ptr<const physics::ShortRangeForce> short_range_force
    )
    {
        // Delegate to factory with full parameters
        return make_integrator(time_delta, bounding_box, std::move(short_range_force), {});
    }

    std::unique_ptr<const Integrator> IntegratorFactory::make_integrator(
        double time_delta,
        tools::BoundingBox bounding_box,
        std::unique_ptr<const physics::ShortRangeForce> short_range_force,
        IntegratorFactory::TypeSelections type_selections
    )
    {
        // Create the boundary conditions
        std::unique_ptr<const BoundaryCondition> boundary_condition;
        switch (type_selections.boundary_condition)
        {
        case BoundaryConditionType::periodic:
            boundary_condition = std::make_unique<const PeriodicBoundaryCondition>(bounding_box);
            break;
        
        default:
            throw std::runtime_error("Unknown BoundaryCondition type");
            break;
        }

        // Create particle pair filter
        std::unique_ptr<ParticlePairFilter> particle_pair_filter;
        switch (type_selections.particle_pair_filter)
        {
        case ParticlePairFilterType::naive:
            particle_pair_filter = std::make_unique<NaiveParticlePairFilter>(
                bounding_box, short_range_force->cutoff_distance()
            );
            break;
            
        case ParticlePairFilterType::cell_list:
            particle_pair_filter = std::make_unique<CellListParticlePairFilter>(
                bounding_box, short_range_force->cutoff_distance()
            );
            break;
        
        default:
            throw std::runtime_error("Unknown ParticlePairFilter type");
            break;
        }

        // Create force calculation
        std::unique_ptr<const ForceCalculation> force_calculation;
        switch (type_selections.force_calculation)
        {   
        case ForceCalculationType::short_range:
            force_calculation = std::make_unique<const ShortRangeForceCalculation>(
                std::move(short_range_force), std::move(particle_pair_filter)
            );
            break;
        
        default:
            throw std::runtime_error("Unknown ForceCalculation type");
            break;
        }

        // Create integrator
        std::unique_ptr<const Integrator> integrator;
        switch (type_selections.integrator)
        {
        case IntegratorType::velocity_verlet:
            integrator = std::make_unique<const VelocityVerletIntegrator>(
                time_delta,
                std::move(boundary_condition),
                std::move(force_calculation)
            );
            break;
        
        default:
            throw std::runtime_error("Unknown Integrator type");
            break;
        }

        // Return
        return integrator;
    }
} // namespace engine

