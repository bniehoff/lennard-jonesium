/**
 * integrator.hpp
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

#ifndef LJ_INTEGRATOR_HPP
#define LJ_INTEGRATOR_HPP

#include <memory>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/force_calculation.hpp>
#include <lennardjonesium/engine/boundary_condition.hpp>

namespace engine
{
    class Integrator
    {
        /**
         * An Integrator is an operator that acts on the SystemState, evolving it forward by one
         * unit of time.  There are many different integrator strategies one could use, which will
         * be implemented in concrete derived classes.  Derived classes should override operator().
         */

        public:
            /**
             * Evolves a SystemState forward by one unit of time.  Should be given a concrete
             * implementation in derived classes.
             */
            virtual physics::SystemState& operator() (physics::SystemState&) const = 0;

            /**
             * Returns a lambda which evolves the SystemState forward `steps` number of times.
             * The reason to return a lambda is it allows a nice way to use the piping syntax:
             * 
             *      s | integrator | integrator(n) | etc...
             */
            virtual physics::SystemState::Operator operator() (int steps) const final;

            /**
             * Create a "default" integrator with the given time_delta, and no interactions or
             * boundary conditions.
             */
            explicit Integrator(double time_delta);

            // Create an integrator with the given time delta, interaction, and boundary condition
            Integrator(
                double time_delta,
                std::unique_ptr<const BoundaryCondition>,
                std::unique_ptr<const ForceCalculation>
            );

            // Make sure dynamically allocated derived classes are properly destroyed
            virtual ~Integrator() = default;

        protected:
            // The time step by which we will increment (assumed fixed)
            const double time_delta_;

            // A state operator that imposes the boundary condition
            std::unique_ptr<const BoundaryCondition> boundary_condition_;

            // A state operator that computes the forces, potential energy, and virial
            std::unique_ptr<const ForceCalculation> force_calculation_;
    };

    // We implement specific integration algorithms as derived classes

    class VelocityVerletIntegrator : public Integrator
    {
        public:
            // Should be able to inherit constructor without problems
            using Integrator::Integrator;

            // Evolves time by one step
            virtual physics::SystemState& operator() (physics::SystemState&) const override;
    };
} // namespace engine

#endif
