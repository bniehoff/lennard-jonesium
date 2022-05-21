"""
sweep_configuration.py

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


from dataclasses import dataclass, field


from lennardjonesium.tools import INIParsable, DictParsable


@dataclass
class SweepConfiguration(INIParsable, DictParsable):
    """
    A SweepConfiguration describes how to generate Configuration objects for a collection of
    related simulations, over a range of temperatures and densities.  This is useful for running
    a large number of jobs in order to generate data for a complete phase diagram.
    """

    @dataclass
    class _System:
        """
        These are mostly the same as the Configuration._System parameters, except that the
        temperature and density parameters describe a np.linspace, and we do not provide a random
        seed.  (A random seed will be obtained/stored separately for each run.)
        """
        temperature_start: float = 0.1
        temperature_stop: float = 1.0
        temperature_steps: int = 10
        density_start: float = 0.1
        density_stop: float = 1.0
        density_steps: int = 10
        particle_count: int = 100
        cutoff_distance: float = 2.5
        time_delta: float = 0.005
    
    @dataclass
    class _Templates:
        """
        These format strings show how the temperature and density values will be used to construct
        the specific directories and SimulationPhase names that will be used for each simulation.
        They should be strings with format fields named `temperature`, `density`, and `name`.
        """
        directory: str = 'T_{temperature:f}/d_{density:f}'
        run_config_file: str = 'run.ini'
        phase_name: str = '(T={temperature:f}, d={density:f}) {name}'
    
    @dataclass
    class _Equilibration:
        name: str = 'Equilibration Phase'
        tolerance: float = 0.05
        sample_size: int = 50
        adjustment_interval: int = 200
        steady_state_time: int = 1000
        timeout: int = 5000
    
    @dataclass
    class _Observation:
        name: str = 'Observation Phase'
        tolerance: float = 0.10
        sample_size: int = 50
        observation_interval: int = 200
        observation_count: int = 20
    
    @dataclass
    class _Filenames:
        """
        These fields should include only the file name to be used, not the directory.  Each output
        file will be automatically placed in a subdirectory given by the `self.templates.directory`
        format string described above.
        """
        event_log: str = 'events.log'
        thermodynamic_log: str = 'thermodynamics.csv'
        observation_log: str = 'observations.csv'
        snapshot_log: str = 'snapshots.csv'
    
    # Since these are mutable, they need to be specified with a default factory
    system: _System = field(default_factory=_System)
    templates: _Templates = field(default_factory=_Templates)
    equilibration: _Equilibration = field(default_factory=_Equilibration)
    observation: _Observation = field(default_factory=_Observation)
    filenames: _Filenames = field(default_factory=_Filenames)
