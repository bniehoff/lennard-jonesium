"""
_simulation.pxd

Copyright (c) 2021-2022 Benjamin E. Niehoff

This file is part of Lennard-Jonesium.

Lennard-Jonesium is free software: you can redistribute
it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later version.

Lennard-Jonesium is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with Lennard-Jonesium.  If not, see
<https://www.gnu.org/licenses/>.
"""


from libcpp cimport bool
from libcpp.memory cimport unique_ptr, shared_ptr
from libcpp.string cimport string

from lennardjonesium.simulation._configuration cimport _Configuration


# Grab what's needed from the TextBuffer class
cdef extern from "<lennardjonesium/tools/text_buffer.hpp>" namespace "tools" nogil:
    cdef cppclass _TextBuffer "tools::TextBuffer":
        _TextBuffer() except +
        string read() except +


# We also need the EchoMode enum
cdef extern from "<lennardjonesium/api/simulation.hpp>" namespace "api" nogil:
    cdef cppclass _EchoMode "api::Simulation::EchoMode":
        pass

cdef extern from "<lennardjonesium/api/simulation.hpp>" namespace "api::Simulation::EchoMode" nogil:
    cdef _EchoMode _silent "api::Simulation::EchoMode::silent"
    cdef _EchoMode _console "api::Simulation::EchoMode::console"
    cdef _EchoMode _buffer "api::Simulation::EchoMode::buffer"


# Grab the declarations we need for the Simulation class
cdef extern from "<lennardjonesium/api/simulation.hpp>" namespace "api" nogil:
    cdef cppclass _Simulation "api::Simulation":
        # Run the simulation asynchronously
        shared_ptr[_TextBuffer] launch()
        shared_ptr[_TextBuffer] launch(_EchoMode)
        void wait()

        # Synchronous wrapper
        void run()

        # Calculate quantities based on particle separation, for plotting
        double potential(double)
        double virial(double)
        double force(double)


# Also grab the factory function needed to create a Simulation from a Configuration
cdef extern from "<lennardjonesium/api/configuration.hpp>" namespace "api" nogil:
    cdef unique_ptr[_Simulation] make_simulation(_Configuration) except +
