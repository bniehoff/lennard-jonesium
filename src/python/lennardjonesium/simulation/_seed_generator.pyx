"""
_seed_generator.pyx

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


from lennardjonesium.simulation._seed_generator cimport _default_seed, _SeedGenerator


cdef class SeedGenerator:
    cdef _SeedGenerator _seed_generator

    cpdef unsigned int get(self): return self._seed_generator.get()

    @staticmethod
    cdef unsigned int default_seed(): return _default_seed

    @staticmethod
    def default_seed(): return _default_seed
