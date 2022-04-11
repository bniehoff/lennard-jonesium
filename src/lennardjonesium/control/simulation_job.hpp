/**
 * simulation_job.hpp
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

#ifndef LJ_SIMULATION_JOB_HPP
#define LJ_SIMULATION_JOB_HPP

#include <memory>
#include <chrono>
#include <future>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/initial_condition.hpp>
#include <lennardjonesium/output/logger.hpp>
#include <lennardjonesium/control/simulation.hpp>

namespace control
{
    class SimulationJob
    {
        /**
         * SimulationJob provides an interface for launching the Simulation asynchronously,
         * and obtaining the result.
         * 
         * TODO: Decide what to do with logging.
         * TODO: Define Parameters/Context and Builder/Factory.
         */

        public:
            // Launch the SimulationJob (or should this happen at construction time?)
            void launch(std::launch policy = std::launch::async)
            {
                final_state_ = std::async(policy, simulation_, initial_state());
            }

            // Expose the wait(), wait_for(), and wait_until() methods of std::future
            void wait() {final_state_.wait();}

            template<class Rep, class Period>
            std::future_status wait_for
                (const std::chrono::duration<Rep, Period>& timeout_duration)
            {
                return final_state_.wait_for<Rep, Period>(timeout_duration);
            }

            template<class Clock, class Duration>
            std::future_status wait_until
                (const std::chrono::time_point<Clock, Duration>& timeout_time)
            {
                return final_state_.wait_until<Clock, Duration>(timeout_time);
            }

            // Access the initial and final state
            physics::SystemState initial_state() {return initial_condition_.system_state();}
            physics::SystemState final_state() {return final_state_.get();}
        
        private:
            output::Logger logger_;
            Simulation simulation_;
            engine::InitialCondition initial_condition_;
            std::future<physics::SystemState> final_state_;
    };
} // namespace control


#endif
