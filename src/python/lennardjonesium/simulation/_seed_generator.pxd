"""
_seed_generator.pxd

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


# Grab the declarations for the SeedGenerator class
cdef extern from "<lennardjonesium/api/seed_generator.hpp>" namespace "api" nogil:
    # Expose the default seed constant separately
    cdef unsigned int _default_seed "api::SeedGenerator::default_seed"

    # Expose the class with default constructor and get() method
    cdef cppclass _SeedGenerator "api::SeedGenerator":
        _SeedGenerator() except +

        # Get a random seed
        unsigned int get() except +
