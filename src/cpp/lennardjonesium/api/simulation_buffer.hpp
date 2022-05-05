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

#ifndef LJ_SIMULATION_BUFFER_HPP
#define LJ_SIMULATION_BUFFER_HPP

#include <memory>
#include <string>
#include <thread>

#include <lennardjonesium/tools/text_buffer.hpp>
#include <lennardjonesium/api/simulation.hpp>

namespace api
{
    class SimulationBuffer
    {
        /**
         * SimulationBuffer is a wrapper class that launches a Simulation asynchronously, keeps
         * track of the thread via RAII, and provides an interface for accessing the output via
         * a read() method.  Its main purpose is to interface with Python.  The Python interpreter
         * has some difficulty syncing stdout between threads (since Python is intended to be run
         * with a Global Interpreter Lock, and the Simulation output on the C++ side is running
         * in a separate thread without this lock).  So, by putting the Events messages in a buffer
         * and allowing Python to explicitly call read(), then Python itself can manage synchronous
         * printing via its own print() function.
         */

        public:
            void launch(Simulation&);
            void wait();
            std::string read();

            ~SimulationBuffer() noexcept;
        
        private:
            std::shared_ptr<tools::TextBuffer> buffer_;
            std::jthread simulation_job_;
    };
} // namespace api


#endif
