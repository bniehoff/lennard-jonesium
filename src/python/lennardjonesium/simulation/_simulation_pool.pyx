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


cdef class SimulationPool:
    """
    SimulationPool is a thread pool with a number of workers for running Simulations.  Simply push()
    the Simulation objects onto the queue, and they will run as soon as a worker is available.  The
    status() method returns useful information about currently-running threads.
    """

    # The Status tuple contains the following entries:
    # The waiting, running, and completed numbers are probably the most interesting
    Status = NamedTuple('Status', [
        ('queued', int),        # Total jobs queued since initialization
        ('waiting', int),       # Number of jobs currently waiting for a worker
        ('started', int),       # Total number of jobs that have been picked up by workers
        ('running', int),       # Number of jobs currently running
        ('completed', int)      # Number of jobs completed
    ])

    def __cinit__(self, thread_count: int = 4):
        """
        NOTE: We do not do any checks as to whether the hardware has enough independent cores to
        run the jobs in parallel.  Apparently it's hard to do this in a platform-independent way.
        Therefore be judicious in terms of how many threads you wish to spawn.
        """
        if not isinstance(thread_count, int):
            raise TypeError('Thread count must be an integer')
        
        cdef int cpp_thread_count = <int> thread_count

        self._cpp_simulation_pool = move[unique_ptr[_SimulationPool]](
            make_unique[_SimulationPool](cpp_thread_count)
        )
    
    cdef _SimulationPool* cpp_simulation_pool(self):
        return self._cpp_simulation_pool.get()
    
    def push(self, simulation: Simulation):
        """
        Pushes a Simulation onto the queue, where it waits until picked up by a worker thread.
        """
        cdef Simulation _simulation = <Simulation?> simulation

        self.cpp_simulation_pool().push(_simulation.cpp_simulation()[0])
    
    def close(self):
        """
        If no further jobs will be pushed, then you can call close() to allow the workers to be
        deallocated when they are finished with the jobs currently in the queue.

        NOTE: If you attempt to push() a Simulation after close() has been called, this is not an
            error; however, the Simulation will NOT be pushed onto the queue.
        """
        self.cpp_simulation_pool().close()
    
    def wait(self):
        """
        Closes the queue to any further submissions, and also waits until all workers are finished
        with all jobs currently in the queue.  This is a blocking call until all work is done.
        """
        self.cpp_simulation_pool().wait()
    
    def status(self):
        """
        Get the current status of the workers.
        """
        cdef _SimulationPool._Status _status = self.cpp_simulation_pool().status()

        return SimulationPool.Status(
            queued=_status.queued,
            waiting=_status.waiting,
            started=_status.started,
            running=_status.running,
            completed=_status.completed
        )
