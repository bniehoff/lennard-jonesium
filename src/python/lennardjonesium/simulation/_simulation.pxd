"""
_simulation.pxd

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


from lennardjonesium.simulation._parameters cimport _SimplifiedParameters


# Grab the declarations we need for the Simulation class
cdef extern from "<lennardjonesium/api/simulation.hpp>" namespace "api" nogil:
    cdef cppclass _FullParameters "api::Simulation::Parameters":
        # We don't expose any further details besides that it can be default constructed
        _FullParameters() except +
    
    cdef cppclass _Simulation "api::Simulation":
        _Simulation(_FullParameters) except +

        # Run the simulation
        void run()

        # Calculate quantities based on particle separation, for plotting
        double potential(double)
        double virial(double)
        double force(double)


# We also need the function that is used to convert SimplifiedParameters to the full
# Simulation::Parameters
cdef extern from "<lennardjonesium/api/parameters.hpp>" namespace "api" nogil:
    cdef _FullParameters make_simulation_parameters(_SimplifiedParameters) except +
