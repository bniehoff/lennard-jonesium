"""
_parameters.pyx

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


from libcpp.string cimport string

from lennardjonesium.simulation._parameters cimport _default_seed, _SimplifiedParameters


cdef class Parameters:
    """
    The Parameters class exposes the SimplifiedParameters to Python.  Would be nice if we had
    Dataclasses in Cython, instead we have to create the getters and setters manually.
    """
    cdef _SimplifiedParameters _parameters

    def __cinit__(self, *,
        double temperature = 0.8,
        double density = 1.0,
        int particle_count = 100,
        unsigned int random_seed = _default_seed,
        double cutoff_distance = 2.5,
        double time_delta = 0.005,
        str equilibration_phase_name = 'Equilibration Phase',
        double equilibration_tolerance = 0.05,
        int equilibration_sample_size = 50,
        int equilibration_adjustment_interval = 200,
        int equilibration_steady_state_time = 1000,
        int equilibration_timeout = 5000,
        str observation_phase_name = 'Observation Phase',
        double observation_tolerance = 0.10,
        int observation_sample_size = 50,
        int observation_interval = 200,
        int observation_count = 20,
        str event_log_path = 'events.log',
        str thermodynamic_log_path = 'thermodynamics.csv',
        str observation_log_path = 'observations.csv',
        str snapshot_log_path = 'snapshots.csv'
    ):
        # Set all values
        self.temperature = temperature
        self.density = density
        self.particle_count = particle_count
        self.random_seed = random_seed
        self.cutoff_distance = cutoff_distance
        self.time_delta = time_delta
        self.equilibration_phase_name = equilibration_phase_name
        self.equilibration_tolerance = equilibration_tolerance
        self.equilibration_sample_size = equilibration_sample_size
        self.equilibration_adjustment_interval = equilibration_adjustment_interval
        self.equilibration_steady_state_time = equilibration_steady_state_time
        self.equilibration_timeout = equilibration_timeout
        self.observation_phase_name = observation_phase_name
        self.observation_tolerance = observation_tolerance
        self.observation_sample_size = observation_sample_size
        self.observation_interval = observation_interval
        self.observation_count = observation_count
        self.event_log_path = event_log_path
        self.thermodynamic_log_path = thermodynamic_log_path
        self.observation_log_path = observation_log_path
        self.snapshot_log_path = snapshot_log_path
    
    # def __dealloc__(self):
    #     del self._parameters

    # A function for accessing the SimplifiedParameters struct directly
    # Note that we cannot use @property on cdef functions
    cdef _SimplifiedParameters data(self): return self._parameters
    
    # Define properties that echo all values
    @property
    def temperature(self): return self._parameters.temperature

    @property
    def density(self): return self._parameters.density

    @property
    def particle_count(self): return self._parameters.particle_count

    @property
    def random_seed(self): return self._parameters.random_seed 

    @property
    def cutoff_distance(self): return self._parameters.cutoff_distance

    @property
    def time_delta(self): return self._parameters.time_delta

    @property
    def equilibration_phase_name(self):
        return str(self._parameters.equilibration_phase_name, 'utf-8')
    
    @property
    def equilibration_tolerance(self): return self._parameters.equilibration_tolerance

    @property
    def equilibration_sample_size(self): return self._parameters.equilibration_sample_size

    @property
    def equilibration_adjustment_interval(self):
        return self._parameters.equilibration_adjustment_interval
    
    @property
    def equilibration_steady_state_time(self):
        return self._parameters.equilibration_steady_state_time
    
    @property
    def equilibration_timeout(self): return self._parameters.equilibration_timeout

    @property
    def observation_phase_name(self): return str(self._parameters.observation_phase_name, 'utf-8')

    @property
    def observation_tolerance(self): return self._parameters.observation_tolerance

    @property
    def observation_sample_size(self): return self._parameters.observation_sample_size

    @property
    def observation_interval(self): return self._parameters.observation_interval

    @property
    def observation_count(self): return self._parameters.observation_count

    @property
    def event_log_path(self): return str(self._parameters.event_log_path, 'utf-8')

    @property
    def thermodynamic_log_path(self): return str(self._parameters.thermodynamic_log_path, 'utf-8')

    @property
    def observation_log_path(self): return str(self._parameters.observation_log_path, 'utf-8')

    @property
    def snapshot_log_path(self): return str(self._parameters.snapshot_log_path, 'utf-8')

    # Setters will also be useful
    @temperature.setter
    def temperature(self, double value): self._parameters.temperature = value

    @density.setter
    def density(self, double value): self._parameters.density = value

    @particle_count.setter
    def particle_count(self, int value): self._parameters.particle_count = value

    @random_seed.setter
    def random_seed(self, unsigned int value): self._parameters.random_seed  = value

    @cutoff_distance.setter
    def cutoff_distance(self, double value): self._parameters.cutoff_distance = value

    @time_delta.setter
    def time_delta(self, double value): self._parameters.time_delta = value

    @equilibration_phase_name.setter
    def equilibration_phase_name(self, str value):
        self._parameters.equilibration_phase_name = bytes(value, 'utf-8')
    
    @equilibration_tolerance.setter
    def equilibration_tolerance(self, double value):
        self._parameters.equilibration_tolerance = value

    @equilibration_sample_size.setter
    def equilibration_sample_size(self, int value):
        self._parameters.equilibration_sample_size = value

    @equilibration_adjustment_interval.setter
    def equilibration_adjustment_interval(self, int value):
        self._parameters.equilibration_adjustment_interval = value
    
    @equilibration_steady_state_time.setter
    def equilibration_steady_state_time(self, int value):
        self._parameters.equilibration_steady_state_time = value
    
    @equilibration_timeout.setter
    def equilibration_timeout(self, int value): self._parameters.equilibration_timeout = value

    @observation_phase_name.setter
    def observation_phase_name(self, str value):
        self._parameters.observation_phase_name = bytes(value, 'utf-8')

    @observation_tolerance.setter
    def observation_tolerance(self, double value): self._parameters.observation_tolerance = value

    @observation_sample_size.setter
    def observation_sample_size(self, int value): self._parameters.observation_sample_size = value

    @observation_interval.setter
    def observation_interval(self, int value): self._parameters.observation_interval = value

    @observation_count.setter
    def observation_count(self, int value): self._parameters.observation_count = value

    @event_log_path.setter
    def event_log_path(self, str value):
        self._parameters.event_log_path = bytes(value, 'utf-8')

    @thermodynamic_log_path.setter
    def thermodynamic_log_path(self, str value):
        self._parameters.thermodynamic_log_path = bytes(value, 'utf-8')

    @observation_log_path.setter
    def observation_log_path(self, str value):
        self._parameters.observation_log_path = bytes(value, 'utf-8')

    @snapshot_log_path.setter
    def snapshot_log_path(self, str value):
        self._parameters.snapshot_log_path = bytes(value, 'utf-8')
