/**
 * simulation_pool.cpp
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
#include <mutex>
#include <thread>
#include <algorithm>
#include <ranges>

#include <lennardjonesium/tools/message_buffer.hpp>
#include <lennardjonesium/api/simulation.hpp>
#include <lennardjonesium/api/simulation_pool.hpp>

namespace api
{
    SimulationPool::SimulationPool(int thread_count)
    {
        // Populate the thread pool
        for (auto i [[maybe_unused]] : std::views::iota(0, thread_count))
        {
            threads_.emplace_back(Worker(*this));
        }
    }

    SimulationPool::Status SimulationPool::status()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return SimulationPool::Status{
            .queued = queued_,
            .waiting = queued_ - started_,
            .started = started_,
            .running = started_ - completed_,
            .completed = completed_
        };
    }

    void SimulationPool::increment_queued_()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ++queued_;
    }

    void SimulationPool::increment_started_()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ++started_;
    }

    void SimulationPool::increment_completed_()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ++completed_;
    }

    void SimulationPool::wait()
    {
        // Should always close before joining threads, or else we will be stuck in an infinite loop
        close();

        for (auto& thread : threads_)
        {
            if (thread.joinable()) {thread.join();}
        }
    }

    SimulationPool::~SimulationPool()
    {
        // Wait for any running simulations to finish
        try {wait();}
        catch (...) {}
    }

    void SimulationPool::Worker::operator() ()
    {
        while (auto job = pool_.jobs_.get())
        {
            pool_.increment_started_();
            job.value().get().run();
            pool_.increment_completed_();
        }
    }
} // namespace api

