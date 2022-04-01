/**
 * fixed_phases.hpp
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

#ifndef LJ_FIXED_PHASES_HPP
#define LJ_FIXED_PHASES_HPP

#include <string>

#include <src/lennardjonesium/physics/measurements.hpp>
#include <src/lennardjonesium/physics/observation.hpp>
#include <src/lennardjonesium/control/command_queue.hpp>
#include <src/lennardjonesium/control/simulation_phase.hpp>

namespace mock
{
    /**
     * We define some "fixed" SimulationPhases which ignore their input and just emit a fixed
     * sequence of Commands.  This allows us to predict exactly what the output will be to the
     * various log files, so that we can test the Simulation properly.
     */

    class SuccessPhase : public control::SimulationPhase
    {
        public:
            virtual void evaluate(
                control::CommandQueue& command_queue,    // Output parameter
                int time_step,
                const physics::ThermodynamicMeasurement& measurement
            ) override;

            SuccessPhase(const std::string& name) : control::SimulationPhase(name) {}
    };

    class FailurePhase : public control::SimulationPhase
    {
        public:
            virtual void evaluate(
                control::CommandQueue& command_queue,    // Output parameter
                int time_step,
                const physics::ThermodynamicMeasurement& measurement
            ) override;

            FailurePhase(const std::string& name) : control::SimulationPhase(name) {}
    };
} // namespace mock


#endif
