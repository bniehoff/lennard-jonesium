/**
 * simulation.hpp
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

#ifndef LJ_SIMULATION_HPP
#define LJ_SIMULATION_HPP

#include <queue>
#include <utility>
#include <memory>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/control/simulation_phase.hpp>

namespace control
{
    class Simulation
    {
        /**
         * The Simulation encapsulates the main loop, and runs the entire simulation on a given
         * initial state.  It follows a schedule of SimulationPhases which make the decisions
         * regarding what to do at each time step.  The Simulation itself is responsible for
         * keeping track of the time step count, as well as pushing relevant data to various
         * message queues.
         * 
         * TODO: Add log/message queue(s)
         */

        public:
            using Schedule = std::queue<std::unique_ptr<SimulationPhase>>;

            physics::SystemState& operator() (physics::SystemState&);

            Simulation(const engine::Integrator& integrator, Schedule schedule)
                : integrator_{integrator}, simulation_phases_{std::move(schedule)}
            {}
        
        private:
            const engine::Integrator& integrator_;
            Schedule simulation_phases_;
    };
} // namespace control


#endif
