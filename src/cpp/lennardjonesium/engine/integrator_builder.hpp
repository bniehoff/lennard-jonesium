/**
 * integrator_builder.hpp
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

#ifndef LJ_INTEGRATOR_BUILDER_HPP
#define LJ_INTEGRATOR_BUILDER_HPP

#include <utility>
#include <memory>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/engine/boundary_condition.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>
#include <lennardjonesium/engine/force_calculation.hpp>
#include <lennardjonesium/engine/integrator.hpp>

namespace detail
{
    // Mixin interface that provides common code to all build steps
    class Buildable
    {
        public:
            template<class IntegratorType = engine::VelocityVerletIntegrator>
            std::unique_ptr<engine::Integrator> build()
            {
                return std::make_unique<IntegratorType>(
                    time_delta_, std::move(boundary_condition_), std::move(force_calculation_)
                );
            }
        
        protected:
            double time_delta_;
            std::unique_ptr<const engine::BoundaryCondition> boundary_condition_;
            std::unique_ptr<const engine::ForceCalculation> force_calculation_;

            Buildable(
                double time_delta,
                std::unique_ptr<const engine::BoundaryCondition> boundary_condition,
                std::unique_ptr<const engine::ForceCalculation> force_calculation
            )
                : time_delta_{time_delta},
                  boundary_condition_{std::move(boundary_condition)},
                  force_calculation_{std::move(force_calculation)}
            {}
    };
} // namespace detail


namespace engine
{
    class Integrator::Builder : public detail::Buildable
    {
        /**
         * Integrator::Builder is responsible for building an Integrator from its most basic
         * ingredients BoundingBox and ShortRangeForce.  Although the Integrator constructor itself
         * takes very few arguments, the process of creating the objects that need to be given in
         * those argumeents can be somewhat complex.  So the Builder class assists in putting all
         * the pieces together.
         * 
         * Since some choices during the build process depend on other choices, we use the Step
         * Builder pattern described here:
         * 
         *      http://rdafbn.blogspot.com/2012/07/step-builder-pattern_28.html
         * 
         * This works by using not just one Builder class, but several, such that the method
         * chaining returns different interfaces each time, which walk you through the process of
         * making a choice from the correct subset of options available at each stage.
         */
        
        private:
            // Next possible steps in the build process will provide new methods
            class WithBoundingBox;

        public:
            // The constructor begins with the only required parameter, the time delta.
            Builder(double time_delta) : detail::Buildable{time_delta, nullptr, nullptr} {}

            template<class BoundaryConditionType = PeriodicBoundaryCondition>
            WithBoundingBox bounding_box(tools::BoundingBox);
    };

    class Integrator::Builder::WithBoundingBox : public detail::Buildable
    {
        private:
            // Next possible steps in the build process will provide new methods
            class WithShortRangeForce;
        
        public:
            WithBoundingBox(
                double time_delta,
                std::unique_ptr<const BoundaryCondition> boundary_condition,
                tools::BoundingBox bounding_box
            )
                : detail::Buildable(time_delta, std::move(boundary_condition), nullptr),
                  bounding_box_{bounding_box}
            {}

            template <class ParticlePairFilterType = CellListParticlePairFilter>
            WithShortRangeForce short_range_force(const physics::ShortRangeForce&);
        
        private:
            // Need to store the bounding box because it might be passed on to the next stage
            tools::BoundingBox bounding_box_;
    };

    class Integrator::Builder::WithBoundingBox::WithShortRangeForce : public detail::Buildable
    {
        public:
            WithShortRangeForce(
                double time_delta,
                std::unique_ptr<const engine::BoundaryCondition> boundary_condition,
                std::unique_ptr<const engine::ForceCalculation> force_calculation
            )
                : detail::Buildable(
                    time_delta, std::move(boundary_condition), std::move(force_calculation)
                )
            {}
    };

    // Now fill in the remaining implementation (must be inline to avoid multiple definitions):
    // Add template specializations for any new types
    template <>
    inline Integrator::Builder::WithBoundingBox
    Integrator::Builder::bounding_box<PeriodicBoundaryCondition>(tools::BoundingBox bounding_box)
    {
        return Integrator::Builder::WithBoundingBox(
            time_delta_,
            std::make_unique<const PeriodicBoundaryCondition>(bounding_box),
            bounding_box
        );
    }

    // In this case, the same code works for both possible instantiations, but in principle
    // we can't assume that every type of particle pair filter is constructed the same way.
    template <class ParticlePairFilterType>
    inline Integrator::Builder::WithBoundingBox::WithShortRangeForce
    Integrator::Builder::WithBoundingBox::short_range_force
        (const physics::ShortRangeForce& short_range_force)
    {
        double cutoff_distance = short_range_force.cutoff_distance();

        return Integrator::Builder::WithBoundingBox::WithShortRangeForce(
            time_delta_,
            std::move(boundary_condition_),
            std::make_unique<const ShortRangeForceCalculation>(
                short_range_force,
                std::make_unique<ParticlePairFilterType>(bounding_box_, cutoff_distance)
            )
        );
    }
} // namespace engine


#endif
