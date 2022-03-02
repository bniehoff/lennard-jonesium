/**
 * simulation_schedule.hpp
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

#ifndef LJ_SIMULATION_SCHEDULE_HPP
#define LJ_SIMULATION_SCHEDULE_HPP

#include <string>
#include <list>
#include <queue>
#include <vector>
#include <variant>
#include <memory>

#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/control/simulation_phase.hpp>

namespace control
{
    /**
     * To inform the SimulationSchedule what we would like it to do, we wrap the parameters in a
     * SimulationPhaseRequest object, which is just a std::variant containing the possible different
     * types of requests.  In this way, each type of request is automatically tagged with the type
     * of SimulationPhase object that needs to be constructed (so, std::variant is roughly doing
     * the job of an enum).
     */
    struct EquilibrationPhaseRequest
    {
        std::string name;
        EquilibrationParameters parameters;
    };

    struct ObservationPhaseRequest
    {
        std::string name;
        ObservationParameters parameters;
    };

    using SimulationPhaseRequest = std::variant<
        EquilibrationPhaseRequest,
        ObservationPhaseRequest
    >;

    class SimulationSchedule
    {
        /**
         * SimulationSchedule manages the creation of SimulationPhase objects at the appropriate
         * time in the simulation.  These objects need to be constructed with their start_time,
         * rather than using some separate init() method to configure them on first use.  My thought
         * was that an init() method might be confusing, because then it would be possible for
         * a SimulationPhase class to be constructed and yet not be in a usable state.  This would
         * violate RAII (although the "resource" in this case is simply the knowledge of the time
         * step on which the SimulationPhase is actually started).
         * 
         * So, to get around this, the SimulationSchedule maintains a list of which SimulationPhase
         * classes have been requested and which Parameters structs should be used to construct
         * them.  Then, at the appropriate time step, a factory method is provided which will
         * dynamically construct the appropriate SimulationPhase and provide it with the initial
         * time step that it needs in order to be ready to function.
         */

        public:
            // Adds a SimulationPhaseRequest to the end of the schedule
            void append(SimulationPhaseRequest request) {schedule_.push(request);};

            // Retrieves the next SimulationPhase using the given start time
            std::unique_ptr<SimulationPhase> next(int start_time);

            // Creates an empty schedule that can be appended to
            SimulationSchedule(tools::SystemParameters system_parameters)
                : system_parameters_{system_parameters}
            {}

            // Or we can initialize the queue directly from a vector
            SimulationSchedule(
                tools::SystemParameters system_parameters,
                std::vector<SimulationPhaseRequest> schedule
            );
        
        private:
            tools::SystemParameters system_parameters_;
            std::queue<SimulationPhaseRequest, std::list<SimulationPhaseRequest>> schedule_{};
    };
} // namespace control


#endif
