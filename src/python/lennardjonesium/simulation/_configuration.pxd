"""
_configuration.pxd

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


# Grab all declarations related to building Configuration structs
cdef extern from "<lennardjonesium/api/configuration.hpp>" namespace "api" nogil:
    cdef cppclass _Configuration "api::Configuration":
        _Configuration() except +

        # Declare the nested structs
        cppclass _System "api::Configuration::System":
            _System() except +

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
        
        cppclass _Equilibration "api::Configuration::Equilibration":
            _Equilibration() except +

            string name
            double tolerance
            int sample_size
            int adjustment_interval
            int steady_state_time
            int timeout
        
        cppclass _Observation "api::Configuration::Observation":
            _Observation() except +

            string name
            double tolerance
            int sample_size
            int observation_interval
            int observation_count
        
        cppclass _Filepaths "api::Configuration::Filepaths":
            _Filepaths() except +

            string event_log
            string thermodynamic_log
            string observation_log
            string snapshot_log
        
        # Now declare the actual member variables
        _System system
        _Equilibration equilibration
        _Observation observation
        _Filepaths filepaths
