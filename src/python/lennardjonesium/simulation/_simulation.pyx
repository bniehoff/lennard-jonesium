"""
_simulation.pyx

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


from lennardjonesium.simulation._simulation cimport _Simulation, make_simulation_parameters

from lennardjonesium.simulation._parameters cimport _SimplifiedParameters
from lennardjonesium.simulation._parameters cimport Parameters


cdef class Simulation:
    """
    The Simulation class ...
    """
    cdef _Simulation* _simulation

    def __cinit__(self, Parameters parameters):
        self._simulation = new _Simulation(
            make_simulation_parameters(
               parameters.data()
            )
        )
    
    def __dealloc__(self):
        del self._simulation
    
    def run(self): self._simulation.run()

    cpdef double potential(self, double separation):
        return self._simulation.potential(separation)

    cpdef double virial(self, double separation):
        return self._simulation.virial(separation)

    cpdef double force(self, double separation):
        return self._simulation.force(separation)
