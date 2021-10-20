/**
 * velocity_verlet_integrator.hpp
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

#ifndef LJ_VELOCITY_VERLET_INTEGRATOR_HPP
#define LJ_VELOCITY_VERLET_INTEGRATOR_HPP

#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/physics/system_state.hpp>

using physics::SystemState;

namespace engine {
    class VelocityVerletIntegrator : public Integrator {
        public:
            // Should be able to inherit constructor without problems
            using Integrator::Integrator;

            // Evolves time by one step
            virtual SystemState& operator() (SystemState&) override;
    };
}

#endif
