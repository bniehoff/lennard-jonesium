"""
_parameters.pxd

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


# Grab the declarations we need for the SimplifiedParameters
cdef extern from "<lennardjonesium/api/parameters.hpp>" namespace "api" nogil:
    # We need this default value, Python doesn't know what it is
    unsigned int _default_seed "api::SimplifiedParameters::default_seed"

    # The C++ class on which we will base our Python Parameters interface
    cdef cppclass _SimplifiedParameters "api::SimplifiedParameters":
        _SimplifiedParameters() except +

        # System parameters
        double temperature
        double density
        int particle_count

        # Random seed
        unsigned int random_seed
        
        # Force parameters
        double cutoff_distance
        
        # Time step size
        double time_delta
        
        # Equilibration parameters
        string equilibration_phase_name
        double equilibration_tolerance
        int equilibration_sample_size
        int equilibration_adjustment_interval
        int equilibration_steady_state_time
        int equilibration_timeout

        # Observation parameters
        string observation_phase_name
        double observation_tolerance
        int observation_sample_size
        int observation_interval
        int observation_count

        # Output file paths
        string event_log_path
        string thermodynamic_log_path
        string observation_log_path
        string snapshot_log_path
