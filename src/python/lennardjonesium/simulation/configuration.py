"""
configuration.py

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


from dataclasses import dataclass

from lennardjonesium.simulation._seed_generator import SeedGenerator


@dataclass
class Configuration:
    @dataclass
    class _System:
        temperature: float = 0.8
        density: float = 1.0
        particle_count: int = 100
        random_seed: int = SeedGenerator.default_seed()
        cutoff_distance: float = 2.5
        time_delta: float = 0.005
    
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
    class _Filepaths:
        event_log: str = 'events.log'
        thermodynamic_log: str = 'thermodynamics.csv'
        observation_log: str = 'observations.csv'
        snapshot_log: str = 'snapshots.csv'
    
    system: _System = _System()
    equilibration: _Equilibration = _Equilibration()
    observation: _Observation = _Observation()
    filepaths: _Filepaths = _Filepaths()
