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
from typing import Iterator
import itertools
import pathlib

from more_itertools import divide

from lennardjonesium.tools import INIParsable, DictParsable, linspace


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

    def sweep_range(self,
        chunk_count: int = 1,
        chunk_index: int = 0,
    ) -> Iterator[tuple[float, float]]:
        """
        Returns an iterator over (temperature, density) pairs for each point in the sweep.
    
        :param chunk_count: Instead of processing all the simulations in the sweep at the same time,
            one can divide them into a number of "chunks" given by this parameter.  This is useful
            on simulation sweeps of large systems that may take a long time.
        
        :param chunk_index: The index of the chunk to process on this run.  Must in the range
            0 <= chunk_index < chunk_count, as it is a list index.
        """
        temperatures = linspace(
            self.system.temperature_start,
            self.system.temperature_stop,
            self.system.temperature_steps,
            endpoint=True
        )

        densities = linspace(
            self.system.density_start,
            self.system.density_stop,
            self.system.density_steps,
            endpoint=True
        )

        yield from list(
            divide(chunk_count, itertools.product(temperatures, densities))
        )[chunk_index]
    
    def simulation_dir(self, temperature, density) -> pathlib.Path:
        """
        Returns the (relative) simulation directory corresponding to a given temperature and
        density.
        """
        return pathlib.Path(self.templates.directory.format(
            temperature=temperature, density=density
        ))
    
    def simulation_dir_range(self,
        chunk_count: int = 1,
        chunk_index: int = 0,
    ) -> Iterator[pathlib.Path]:
        """
        Returns an iterator over all simulation directories.
    
        :param chunk_count: Instead of processing all the simulations in the sweep at the same time,
            one can divide them into a number of "chunks" given by this parameter.  This is useful
            on simulation sweeps of large systems that may take a long time.
        
        :param chunk_index: The index of the chunk to process on this run.  Must in the range
            0 <= chunk_index < chunk_count, as it is a list index.
        """
        return (self.simulation_dir(*td_pair)
                for td_pair in self.sweep_range(chunk_count, chunk_index))
