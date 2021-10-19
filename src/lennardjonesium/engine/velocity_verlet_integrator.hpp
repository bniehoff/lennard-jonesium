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
#include <lennardjonesium/engine/system_state.hpp>

namespace engine {
    class VelocityVerletIntegrator : public Integrator {
        public:
            /**
             * Evolves time by one time step.
             */
            virtual SystemState& forward_step(SystemState&) override;
    };
}

#endif
