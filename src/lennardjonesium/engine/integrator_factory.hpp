/**
 * integrator_factory.hpp
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

#ifndef LJ_INTEGRATOR_FACTORY_HPP
#define LJ_INTEGRATOR_FACTORY_HPP

#include <memory>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/engine/boundary_condition.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>
#include <lennardjonesium/engine/force_calculation.hpp>
#include <lennardjonesium/engine/integrator.hpp>

namespace engine
{
    /**
     * These enum classes define the various options one has for how an integrator can be
     * constructed.  Note that the list is heavily restrictive; but we use the enums in case we
     * might want to expand the possibilities later (for example, reflecting boundary conditions,
     * or Euler integration, etc.)
     * 
     * TODO: If we want to expand the types of ForceCalculation allowed (for example, to include
     * background forces such as gravity), then we will have to completely re-evaluate the structure
     * of this (one possibility is the "fluent builder pattern").  For now, we will not worry about
     * this possibility, since our only goal is to run Lennard-Jones simulations with periodic
     * boundary conditions.
     */

    enum class BoundaryConditionType
    {
        periodic
    };

    enum class ParticlePairFilterType
    {
        naive,
        cell_list
    };

    enum class ForceCalculationType
    {
        short_range
    };

    enum class IntegratorType
    {
        velocity_verlet
    };

    class IntegratorFactory
    {
        /**
         * IntegratorFactory provides static functions for constructing Integrators from some
         * parameters, which simplifies the multi-step process that would otherwise be required.
         * 
         * IntegratorFactory cannot be instantiated; we adopt a more Java-like idiom which collects
         * a number of static functions under a "class" which is really more of a namespace.
         */

        public:
            struct TypeSelections
            {
                /**
                 * The TypeSelections struct collects together the various (polymorphic) types
                 * which might be requested when building an Integrator.
                 */

                BoundaryConditionType boundary_condition = BoundaryConditionType::periodic;
                ParticlePairFilterType particle_pair_filter = ParticlePairFilterType::cell_list;
                ForceCalculationType force_calculation = ForceCalculationType::short_range;
                IntegratorType integrator = IntegratorType::velocity_verlet;
            };

            static std::unique_ptr<const Integrator> make_integrator(
                double time_delta,
                tools::BoundingBox bounding_box,
                std::unique_ptr<const physics::ShortRangeForce> short_range_force,
                TypeSelections type_selections
            );

            // We have to write an explicit overload due to GCC bug regarding nested class defaults
            static std::unique_ptr<const Integrator> make_integrator(
                double time_delta,
                tools::BoundingBox bounding_box,
                std::unique_ptr<const physics::ShortRangeForce> short_range_force
            );

            // We do not allow any instances to be created
            IntegratorFactory() = delete;
    };
} // namespace engine


#endif
