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
    The Simulation class is represents a single instance of a Lennard-Jones fluid for some given
    set of parameters, which are passed to the constructor in a Configuration object.  The main
    method provided is run(), which runs the simulation (either with output to stdout, or silently,
    but in any case the data collected will be printed to the various filenames passed in the
    Configuration object).

    Also provided are three methods potential(), virial(), and force(), which can be used to create
    plots of the functions which determine the physics.
    """

    def __cinit__(self, configuration: Configuration = None):
        if configuration is None:
            configuration = Configuration()
        
        # Build the C++ Configuration object
        cdef _Configuration cpp_configuration = make_cpp_configuration(configuration)
        
        # Create the C++ Simulation object
        self._cpp_simulation = move[unique_ptr[_Simulation]](make_simulation(cpp_configuration))
    
    # def __dealloc__(self):
    #     del self._simulation

    cdef _Simulation* cpp_simulation(self):
        # Get direct access to (a pointer to) the C++ Simulation object
        return self._cpp_simulation.get()

    def run(self, *, echo: bool = True, delay: float = 0.05):
        """
        This is the main method of the Simulation class which actually runs the simulation.  You
        can call this method as often as you like; it will re-run the simulation and overwrite the
        output files.

        :param echo: Whether to print Events log entries to the terminal, as they are printed to
            the Events file
        
        :param delay: If printing to the terminal, the time to wait between print calls.
            NOTE: The self._cpp_simulation_buffer.read() call below is blocking, so it is actually
                OK to set delay=0, and this won't interfere with the thread running the simulation.
                However, a small, non-zero value can help improve the user experience in a Jupyter
                notebook, since the mechanism used by IPyKernel to capture stdout includes a buffer
                of around 100 characters, which causes whole blocks of text to appear at once, if
                there is no delay.
        """
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
        """
        Computes the potential (energy) between two particles at the given separation.
        """
        return self.cpp_simulation().potential(separation)

    cpdef double virial(self, double separation):
        """
        Computes the virial contribution of two particles at a given separation.
        """
        return self.cpp_simulation().virial(separation)

    cpdef double force(self, double separation):
        """
        Computes the z-component of the force between two particles, at the given separation along
        the z-axis.
        """
        return self.cpp_simulation().force(separation)


cdef _Configuration make_cpp_configuration(py_configuration: Configuration):
    """
    Since Cython has not yet implemented the @dataclass decorator, we were forced to implement the
    Python-facing Configuration class in pure Python.  This function takes a Python Configuration
    dataclass object and uses its members to construct a C++ Configuration object.

    NOTE: There are some caveats here regarding converting filepaths to bytes().  In principle this
        might cause problems with Unicode filepaths.  I have not experimented.
    """
    # Builds a C++ Configuration object from a Python Configuration object
    if not isinstance(py_configuration, Configuration):
        raise TypeError('Argument must be of type Configuration')

    cdef _Configuration cpp_configuration = _Configuration()
        
    # System settings
    cpp_configuration.system.temperature = py_configuration.system.temperature
    cpp_configuration.system.density = py_configuration.system.density
    cpp_configuration.system.particle_count = py_configuration.system.particle_count
    cpp_configuration.system.random_seed = py_configuration.system.random_seed
    cpp_configuration.system.cutoff_distance = py_configuration.system.cutoff_distance
    cpp_configuration.system.time_delta = py_configuration.system.time_delta

    # Equilibration settings
    cpp_configuration.equilibration.name = bytes(py_configuration.equilibration.name, 'utf-8')
    cpp_configuration.equilibration.tolerance = py_configuration.equilibration.tolerance
    cpp_configuration.equilibration.sample_size = py_configuration.equilibration.sample_size
    cpp_configuration.equilibration.adjustment_interval = \
        py_configuration.equilibration.adjustment_interval
    cpp_configuration.equilibration.steady_state_time = \
        py_configuration.equilibration.steady_state_time
    cpp_configuration.equilibration.timeout = py_configuration.equilibration.timeout

    # Observation settings
    cpp_configuration.observation.name = bytes(py_configuration.observation.name, 'utf-8')
    cpp_configuration.observation.tolerance = py_configuration.observation.tolerance
    cpp_configuration.observation.sample_size = py_configuration.observation.sample_size
    cpp_configuration.observation.observation_interval = \
        py_configuration.observation.observation_interval
    cpp_configuration.observation.observation_count = \
        py_configuration.observation.observation_count

    # Output files
    cpp_configuration.filepaths.event_log = \
        bytes(py_configuration.filepaths.event_log, 'utf-8')
    cpp_configuration.filepaths.thermodynamic_log = \
        bytes(py_configuration.filepaths.thermodynamic_log, 'utf-8')
    cpp_configuration.filepaths.observation_log = \
        bytes(py_configuration.filepaths.observation_log, 'utf-8')
    cpp_configuration.filepaths.snapshot_log = \
        bytes(py_configuration.filepaths.snapshot_log, 'utf-8')
    
    return cpp_configuration
