/**
 * force_calculation.hpp
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

#ifndef LJ_FORCE_CALCULATION_HPP
#define LJ_FORCE_CALCULATION_HPP

#include <memory>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/engine/particle_pair_filter.hpp>

namespace engine
{   
    class ForceCalculation
    {
        /**
         * ForceCalculation will take the current SystemState and calculate all the forces (and
         * related dynamical quantities, such as potential energy and virial). 
         * 
         * The particular concrete implementations of ForceCalculation and BoundaryCondition are
         * not independent.  For example, a constant background force (like gravity) does not make
         * sense with periodic boundary conditons, which are intended to represent an infinite
         * system.  Even with short-range forces in a finite system, one needs to use different
         * computation methods in the case of periodic boundary conditions (infinite system) vs.
         * reflecting boundary conditions (particles trapped in a literal box).
         * 
         * To make the code fully generic while allowing runtime control over the types of problems
         * being simulated (useful if we want to provide that feature in the Python interface),
         * then we should consider implementing this "parallel dependency" via the Abstract Factory
         * pattern.  However, this is a bit complicated and is beyond the scope of this project.
         */

        public:
            // Compute the forces resulting from this interaction
            virtual physics::SystemState& operator() (physics::SystemState&) const = 0;

            // Make sure dynamically allocated derived classes are properly destroyed
            virtual ~ForceCalculation() = default;
    };

    // Create a type that calculates no forces
    class NullForceCalculation : public ForceCalculation
    {
        public:
            virtual physics::SystemState& operator() (physics::SystemState& s) const override
            {return s;}
    };

    // The following derived classes will also be useful

    class ShortRangeForceCalculation : public ForceCalculation
    {
        public:
            ShortRangeForceCalculation(
                std::unique_ptr<const physics::ShortRangeForce> short_range_force,
                std::unique_ptr<ParticlePairFilter> particle_pair_filter
            );

            // Compute the forces resulting from this interaction
            virtual physics::SystemState& operator() (physics::SystemState&) const override;
        
        private:
            std::unique_ptr<const physics::ShortRangeForce> short_range_force_;

            // Can't be const, because some types use internal state
            std::unique_ptr<ParticlePairFilter> particle_pair_filter_;
    };

    class BackgroundForceCalculation : public ForceCalculation
    {
        public:
            BackgroundForceCalculation(std::unique_ptr<const physics::BackgroundForce>);

            // Compute the forces resulting from this interaction
            virtual physics::SystemState& operator() (physics::SystemState&) const override;
        
        private:
            std::unique_ptr<const physics::BackgroundForce> background_force_;
    };
} // namespace engine

#endif