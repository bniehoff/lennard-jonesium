/**
 * integrator.hpp
 * 
 * Copyright (c) 2021 Benjamin E. Niehoff
 * 
 * This file is part of Lennard-Jones-Particles.
 * 
 * Lennard-Jones-Particles is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Lennard-Jones-Particles is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Lennard-Jones-Particles.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LJ_INTEGRATOR_HPP
#define LJ_INTEGRATOR_HPP

#include <lennardjones/engine/system_state.hpp>

namespace engine {
    class Integrator {
        /**
         * Integrator is an abstract class that represents how we act on the SystemState to advance
         * it through time.  There are many different integrator strategies one could use, which
         * will be implemented in concrete derived classes.
         */
        protected:
            /**
             * We store the time step, as we only consider integrators that step by the same
             * time interval each iteration.  It is possible to change the time resolution from
             * one step to the next, but this is an additional complication that is not useful for
             * this project.
             */
            const double timestep_;
        
        public:
            Integrator(double timestep) : timestep_{timestep}
            {}

            /**
             * Evolves time by one time step.  Must be given concrete implementation.
             */
            virtual Integrator& forward_step(SystemState&) = 0;
    };
}

#endif
