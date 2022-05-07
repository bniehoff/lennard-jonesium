"""
_simulation_pool.pxd

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


from libcpp.memory cimport unique_ptr

from lennardjonesium.simulation._simulation cimport _Simulation


# Declarations for SimulationPool
cdef extern from "<lennardjonesium/api/simulation_pool.hpp>" namespace "api" nogil:
    cdef cppclass _SimulationPool "api::SimulationPool":
        cppclass _Status "Status":
            _Status() except +
            int queued
            int waiting
            int started
            int running
            int completed
        
        void push(_Simulation&) except +
        void close() except +
        void wait() except +

        _Status status() except +


# C++ declarations for SimulationPool
cdef class SimulationPool:
    cdef unique_ptr[_SimulationPool] _cpp_simulation_pool

    cdef _SimulationPool* cpp_simulation_pool(self)
