/**
 * simulation_buffer.hpp
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

#include <memory>
#include <string>
#include <thread>

#include <boost/iostreams/chain.hpp>

#include <lennardjonesium/tools/text_buffer.hpp>
#include <lennardjonesium/api/simulation.hpp>
#include <lennardjonesium/api/simulation_buffer.hpp>

namespace api
{
    void SimulationBuffer::launch(Simulation& simulation)
    {
        // If the simulation is already running, wait for it to finish
        wait();

        // Create the TextBuffer which will be used for output
        buffer_ = std::make_shared<tools::TextBuffer>();

        // Start the simulation job
        simulation_job_ = std::jthread(
            // Copy the buffer pointer to share ownership
            [buffer=buffer_, &simulation]() mutable {
                // Create the echo chain
                Simulation::echo_chain_type echo_chain{};
                echo_chain.push(tools::TextBufferFilter(*buffer));

                // Run the simulation
                simulation.run(echo_chain);

                // Free our copy of the shared buffer (should happen automatically)
                buffer.reset();
            }
        );
    }

    void SimulationBuffer::wait()
    {
        // If a simulation job is running, then wait for it to end
        if (simulation_job_.joinable())
        {
            simulation_job_.join();
        }
    }

    std::string SimulationBuffer::read()
    {
        if (buffer_)
        {
            auto s = buffer_->read();

            // If the buffer is exhausted, we can deallocate it
            if (s.empty()) {buffer_.reset();}

            return s;
        }
        else
        {
            return {};
        }
    }

    SimulationBuffer::~SimulationBuffer()
    {
        // Wait for any running simulations to finish
        try {wait();}
        catch (...) {}
    }
} // namespace api

