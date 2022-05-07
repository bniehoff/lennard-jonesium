"""
_simulation_pool.pyx

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


# cimports
from libcpp.memory cimport unique_ptr, make_unique
from libcpp.utility cimport move

from lennardjonesium.simulation._simulation cimport _Simulation, Simulation
from lennardjonesium.simulation._simulation_pool cimport _SimulationPool


# imports
from typing import NamedTuple

# from lennardjonesium.simulation._simulation import Simulation


cdef class SimulationPool:
    Status = NamedTuple('Status', [
        ('queued', int),
        ('waiting', int),
        ('started', int),
        ('running', int),
        ('completed', int)
    ])

    def __cinit__(self, thread_count: int = 4):
        if not isinstance(thread_count, int):
            raise TypeError('Thread count must be an integer')
        
        cdef int cpp_thread_count = <int> thread_count

        self._cpp_simulation_pool = move[unique_ptr[_SimulationPool]](
            make_unique[_SimulationPool](cpp_thread_count)
        )
    
    cdef _SimulationPool* cpp_simulation_pool(self):
        return self._cpp_simulation_pool.get()
    
    def push(self, simulation: Simulation):
        cdef Simulation _simulation = <Simulation?> simulation

        self.cpp_simulation_pool().push(_simulation.cpp_simulation()[0])
    
    def close(self):
        self.cpp_simulation_pool().close()
    
    def wait(self):
        self.cpp_simulation_pool().wait()
    
    def status(self):
        cdef _SimulationPool._Status _status = self.cpp_simulation_pool().status()

        return SimulationPool.Status(
            queued=_status.queued,
            waiting=_status.waiting,
            started=_status.started,
            running=_status.running,
            completed=_status.completed
        )
