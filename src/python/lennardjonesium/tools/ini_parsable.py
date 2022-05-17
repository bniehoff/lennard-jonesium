"""
ini_parsable.py

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


import configparser


class INIParsable:
    """
    INIParsable is a mixin class that provides the functionality for dataclasses to read and write
    their content to and from .ini files.

    The fields of the dataclass must already be defined; we will not create an entirely new
    dataclass type from whatever arbitrary fields might exist in the .ini file.
    """
    
    @classmethod
    def from_file(cls, filename: str):
        """
        Creates a dataclass object from a config file
        """
        dc = cls()
        dc.read(filename)
        return dc

    def read(self, filename: str):
        """
        Reads a dataclass from a file.
        """
        parser = configparser.ConfigParser()
        parser.read(filename)
        self._read_config_parser(parser)
    
    def write(self, filename: str):
        """
        Writes a dataclass to a file
        """
        parser = configparser.ConfigParser()

        for section in self.__dict__:
            parser.add_section(section)
            for key in self.__getattribute__(section).__dict__:
                parser[section][key] = str(self.__getattribute__(section).__getattribute__(key))
        
        with open(filename, 'w') as outfile:
            parser.write(outfile)
    
    def _read_config_parser(self, cp: configparser.ConfigParser):
        """
        Sets the fields based on values from a ConfigParser.
        """
        # We only look for the keys that are actually defined in the dataclass, so if the
        # ConfigParser contains additional keys, they will be ignored
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
