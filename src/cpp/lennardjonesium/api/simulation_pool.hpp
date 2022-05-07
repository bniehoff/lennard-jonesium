/**
 * simulation_pool.hpp
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

#ifndef LJ_SIMULATION_POOL_HPP
#define LJ_SIMULATION_POOL_HPP

#include <vector>
#include <mutex>
#include <thread>
#include <functional>

#include <lennardjonesium/tools/message_buffer.hpp>
#include <lennardjonesium/api/simulation.hpp>

namespace api
{
    class SimulationPool
    {
        /**
         * SimulationPool is a thread pool for running batches of simulations in parallel.  The
         * simulation jobs themselves are run without printing anything to stdout.  One should also
         * take care that the simulation jobs are given different filepaths for the output files.
         */

        public:
            // Add a simulation job to the queue
            void push(Simulation& simulation)
            {
                // Need to increment the count first, or else it's possible that started_ could be
                // incremented before queued_
                increment_queued_();
                jobs_.put(simulation);
            }

            // Indicate we are done pushing jobs to the queue (will shut down the workers after they
            // finish their current jobs).
            void close() {jobs_.close();}

            // Wait for all jobs to finish
            void wait();

            // Status struct allows us to get information about the queued jobs
            struct Status
            {
                int queued;     // Total number of jobs queued since initialization
                int waiting;    // Number of jobs queued and still waiting to be started
                int started;    // Total number of jobs which have been started since initialization
                int running;    // Number of jobs currently running
                int completed;  // Number of jobs completed
            };

            // Get the current Status
            Status status();

            // We initialize the SimulationPool with the number of threads to use
            explicit SimulationPool(int thread_count = 4);

            // Waits for any remaining jobs to finish before destruction
            ~SimulationPool() noexcept;
        
        private:
            // Worker is a function object that runs in a thread and processes Simulations
            class Worker
            {
                public:
                    Worker(SimulationPool& pool) : pool_{pool} {}

                    void operator() ();
                
                private:
                    SimulationPool& pool_;
            };

            // The actual thread pool
            std::vector<std::jthread> threads_;

            // The job queue
            tools::MessageBuffer<std::reference_wrapper<Simulation>> jobs_;

            // These track the state of the queue
            std::mutex mutex_;
            int queued_{};
            int started_{};
            int completed_{};

            // These wrap mutex accesses for changing the state
            void increment_queued_();
            void increment_started_();
            void increment_completed_();
    };
} // namespace api


#endif
