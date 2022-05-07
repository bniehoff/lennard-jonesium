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


# cimports
from libcpp.memory cimport unique_ptr, make_unique
from libcpp.utility cimport move
from libcpp.string cimport string

from lennardjonesium.simulation._configuration cimport _Configuration
from lennardjonesium.simulation._simulation cimport (
    _Simulation,
    _SimulationBuffer,
    make_simulation
)

# imports
import time

from lennardjonesium.simulation.configuration import Configuration


cdef class Simulation:
    """
    The Simulation class ...
    """

    def __cinit__(self, configuration = None):
        if configuration is None:
            configuration = Configuration()

        if (not isinstance(configuration, Configuration)):
            raise TypeError('Initialization argument must be of type Configuration')
        
        # Build the C++ Configuration object
        # TODO: Maybe there is a way for the Python Configuration class to do this
        cdef _Configuration cpp_configuration = _Configuration()
        
        # System settings
        cpp_configuration.system.temperature = configuration.system.temperature
        cpp_configuration.system.density = configuration.system.density
        cpp_configuration.system.particle_count = configuration.system.particle_count
        cpp_configuration.system.random_seed = configuration.system.random_seed
        cpp_configuration.system.cutoff_distance = configuration.system.cutoff_distance
        cpp_configuration.system.time_delta = configuration.system.time_delta

        # Equilibration settings
        cpp_configuration.equilibration.name = bytes(configuration.equilibration.name, 'utf-8')
        cpp_configuration.equilibration.tolerance = configuration.equilibration.tolerance
        cpp_configuration.equilibration.sample_size = configuration.equilibration.sample_size
        cpp_configuration.equilibration.adjustment_interval = \
            configuration.equilibration.adjustment_interval
        cpp_configuration.equilibration.steady_state_time = \
            configuration.equilibration.steady_state_time
        cpp_configuration.equilibration.timeout = configuration.equilibration.timeout

        # Observation settings
        cpp_configuration.observation.name = bytes(configuration.observation.name, 'utf-8')
        cpp_configuration.observation.tolerance = configuration.observation.tolerance
        cpp_configuration.observation.sample_size = configuration.observation.sample_size
        cpp_configuration.observation.observation_interval = \
            configuration.observation.observation_interval
        cpp_configuration.observation.observation_count = \
            configuration.observation.observation_count

        # Output files
        cpp_configuration.filepaths.event_log = \
            bytes(configuration.filepaths.event_log, 'utf-8')
        cpp_configuration.filepaths.thermodynamic_log = \
            bytes(configuration.filepaths.thermodynamic_log, 'utf-8')
        cpp_configuration.filepaths.observation_log = \
            bytes(configuration.filepaths.observation_log, 'utf-8')
        cpp_configuration.filepaths.snapshot_log = \
            bytes(configuration.filepaths.snapshot_log, 'utf-8')

        # Create the C++ Simulation object
        self._cpp_simulation = move[unique_ptr[_Simulation]](make_simulation(cpp_configuration))
    
    # def __dealloc__(self):
    #     del self._simulation

    cdef _Simulation* cpp_simulation(self):
        # Get direct access to (a pointer to) the C++ Simulation object
        return self._cpp_simulation.get()

    def run(self, *, echo: bool = True, delay: float = 0.05):
        if echo:
            # We have to use the asynchronous API and Python's print() function
            self._cpp_simulation_buffer.launch(self.cpp_simulation()[0])
            line = str(self._cpp_simulation_buffer.read(), 'utf-8')
            while line:
                print(line, flush=True)
                time.sleep(delay)
                line = str(self._cpp_simulation_buffer.read(), 'utf-8')
            self._cpp_simulation_buffer.wait()
        else:
            self.cpp_simulation().run(_Simulation._Echo.Silent())

    cpdef double potential(self, double separation):
        return self.cpp_simulation().potential(separation)

    cpdef double virial(self, double separation):
        return self.cpp_simulation().virial(separation)

    cpdef double force(self, double separation):
        return self.cpp_simulation().force(separation)
