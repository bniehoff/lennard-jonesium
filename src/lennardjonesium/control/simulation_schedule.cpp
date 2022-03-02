/**
 * simulation_schedule.cpp
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

#include <vector>
#include <variant>
#include <memory>

#include <lennardjonesium/control/simulation_phase.hpp>
#include <lennardjonesium/control/simulation_schedule.hpp>

namespace
{
    // Used for writing elegant std::visitor, anonymous namespace for file scope only
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
} // namespace


namespace control
{
    SimulationSchedule::SimulationSchedule
    (tools::SystemParameters system_parameters, std::vector<SimulationPhaseRequest> schedule)
        : SimulationSchedule(system_parameters)
    {
        for (auto request : schedule)
        {
            schedule_.push(request);
        }
    }

    std::unique_ptr<SimulationPhase> SimulationSchedule::next(int start_time)
    {
        if (schedule_.empty()) {return nullptr;}
        
        const auto& request = schedule_.front();

        auto simulation_phase = std::visit(
            overloaded {
                [this, start_time](const EquilibrationPhaseRequest& request)
                    -> std::unique_ptr<SimulationPhase>
                {
                    return std::make_unique<EquilibrationPhase>(
                        request.name,
                        start_time,
                        this->system_parameters_,
                        request.parameters
                    );
                },
                [this, start_time](const ObservationPhaseRequest& request)
                    -> std::unique_ptr<SimulationPhase>
                {
                    return std::make_unique<ObservationPhase>(
                        request.name,
                        start_time,
                        this->system_parameters_,
                        request.parameters
                    );
                }
            },
            request
        );

        schedule_.pop();

        return simulation_phase;
    }
} // namespace control

