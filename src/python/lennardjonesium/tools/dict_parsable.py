"""
dict_parsable.py

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


class DictParsable:
    """
    DictParsable is a mixin class that provides the functionality for dataclasses to read and write
    their content to and from dictionaries.

    The fields of the dataclass must already be defined; we will not create an entirely new
    dataclass type from whatever arbitrary fields might exist in the dictionary.
    """

    @classmethod
    def from_dict(cls, d: dict):
        """
        Creates a dataclass object from a nested dictionary.
        """
        dc = cls()
        dc._read_dict(d)
        return dc
    
    def to_dict(self):
        """
        Creates a nested dictionary from the fields in this dataclass.
        """
        d = {}
        for section in self.__dict__:
            d[section] = {}
            for key in self.__getattribute__(section).__dict__:
                d[section][key] = self.__getattribute__(section).__getattribute__(key)
        return d
    
    def _read_dict(self, d: dict):
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
