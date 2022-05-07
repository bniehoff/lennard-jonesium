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


# Grab the declarations we need for the Simulation class
cdef extern from "<lennardjonesium/api/simulation.hpp>" namespace "api" nogil:
    cdef cppclass _Simulation "api::Simulation":
        cppclass echo_chain_type:
            pass
        
        cppclass _Echo "Echo":
            @staticmethod
            echo_chain_type Silent()

            @staticmethod
            echo_chain_type Console()

        # Run synchronously
        void run(echo_chain_type)

        # Calculate quantities based on particle separation, for plotting
        double potential(double)
        double virial(double)
        double force(double)


# For launching simulations with output synchronized with Python's print(), we need the
# SimulationBuffer class
cdef extern from "<lennardjonesium/api/simulation_buffer.hpp>" namespace "api" nogil:
    cdef cppclass _SimulationBuffer "api::SimulationBuffer":
        void launch(_Simulation&)
        void wait()
        string read()


# Also grab the factory function needed to create a Simulation from a Configuration
cdef extern from "<lennardjonesium/api/configuration.hpp>" namespace "api" nogil:
    cdef unique_ptr[_Simulation] make_simulation(_Configuration) except +


# C++ declarations for Cython Simulation class
cdef class Simulation:
    cdef _SimulationBuffer _cpp_simulation_buffer
    cdef unique_ptr[_Simulation] _cpp_simulation

    # Get the internal _simulation
    cdef _Simulation* cpp_simulation(self)

    # Physics functions
    cpdef double potential(self, double)
    cpdef double virial(self, double)
    cpdef double force(self, double)
