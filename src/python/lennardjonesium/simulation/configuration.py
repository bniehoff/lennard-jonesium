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
import configparser

from lennardjonesium.simulation._seed_generator import SeedGenerator


@dataclass
class Configuration:
    """
    The Configuration holds all of the parameters needed to run a single Simulation.  These are
    stored in a convenient nested dataclass structure.  We also provide some methods for reading
    and writing config files, as well as conversion to a Configuration from a simple nested
    dictionary.
    """
    @dataclass
    class _System:
        temperature: float = 0.8
        density: float = 1.0
        particle_count: int = 100
        cutoff_distance: float = 2.5
        time_delta: float = 0.005
        random_seed: int = SeedGenerator.default_seed()
    
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

    @classmethod
    def from_dict(cls, d: dict):
        """
        Creates a Configuration object from a nested dictionary.
        """
        c = cls()
        c.read_dict(d)
        return c
    
    @classmethod
    def from_config_parser(cls, cp: configparser.ConfigParser):
        """
        Creates a Configuration object from a ConfigParser object.
        """
        c = cls()
        c.read_config_parser(cp)
        return c
    
    @classmethod
    def from_file(cls, filename: str):
        """
        Creates a Configuration object from a config file
        """
        c = cls()
        c.read(filename)
        return c
    
    def read_dict(self, d: dict):
        """
        Sets the fields based on values from a dictionary.
        """
        # We only look for the keys that are actually defined in the dataclass, so if the
        # dictionary contains additional keys, they will be ignored
        for section in self.__dict__:
            if section in d:
                for key in self.__getattribute__(section).__dict__:
                    if key in d[section]:
                        self.__getattribute__(section).__setattr__(key, d[section][key])
    
    def read_config_parser(self, cp: configparser.ConfigParser):
        """
        Sets the fields based on values from a ConfigParser (not quite the same as a dictionary).
        """
        # We only look for the keys that are actually defined in the dataclass, so if the
        # dictionary contains additional keys, they will be ignored
        for section in self.__dict__:
            if section in cp:
                for key in self.__getattribute__(section).__dict__:
                    if key in cp[section]:
                        # We need to use different methods to automatically parse the values into
                        # the appropriate type
                        field_type = type(self.__getattribute__(section).__getattribute__(key))

                        if field_type is int:
                            value = cp.getint(section, key)
                        elif field_type is float:
                            value = cp.getfloat(section, key)
                        elif field_type is bool:
                            value = cp.getboolean(section, key)
                        else:
                            value = cp[section][key]

                        self.__getattribute__(section).__setattr__(key, value)

    def read(self, filename: str):
        """
        Reads a Configuration from a file.

        The filepaths for the log files will be read literally from the config file, whether they
        are absolute or relative paths.  This works fine if one is running LennardJonesium from the
        same directory where the config file is located.  In other cases, one probably wants a
        relative path to be interpreted as relative to the location of the config file, rather than
        from the working directory where Python was launched.  This circumstance will be handled
        elsewhere.
        """
        parser = configparser.ConfigParser()
        parser.read(filename)
        self.read_config_parser(parser)
    
    def write(self, filename: str):
        """
        Writes a Configuration to a file
        """
        parser = configparser.ConfigParser()

        for section in self.__dict__:
            parser.add_section(section)
            for key in self.__getattribute__(section).__dict__:
                parser[section][key] = str(self.__getattribute__(section).__getattribute__(key))
        
        with open(filename, 'w') as outfile:
            parser.write(outfile)
