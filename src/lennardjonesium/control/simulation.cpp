/**
 * simulation.cpp
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

#include <variant>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/integrator.hpp>
#include <lennardjonesium/control/simulation_phase.hpp>
#include <lennardjonesium/control/simulation.hpp>

namespace control
{
    physics::SystemState& Simulation::operator() (physics::SystemState& state)
    {
        // Clock for counting the global time
        int time_step = 0;

        // Initialize the first SimulationPhase
        simulation_phases_.front()->set_start_time(time_step);

        // Main simulation loop
        while (true)
        {
            break;
        }

        return state;
    }
} // namespace control
