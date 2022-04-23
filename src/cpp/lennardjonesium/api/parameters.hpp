/**
 * parameters.hpp
 * 
 * Copyright (c) 2021-2022 Benjamin E. Niehoff
 * 
 * This file is part of Lennard-Jonesium.
 * 
 * Lennard-Jonesium is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Lennard-Jonesium is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Lennard-Jonesium.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LJ_PARAMETERS_HPP
#define LJ_PARAMETERS_HPP

#include <random>
#include <string>

#include <lennardjonesium/engine/initial_condition.hpp>
#include <lennardjonesium/api/simulation.hpp>

namespace api
{
    struct SimplifiedParameters
    {
        /**
         * The Simulation::Parameters struct which the Simulation is expecting involves using some
         * Standard Library types that Cython doesn't know how to deal with (variant, filesystem).
         * Also, the nesting of various other structs inside the Simulation::Parameters struct
         * would make it cumbersome to declare and use from Cython.  So, we define a simplified
         * struct which uses a more limited set of parameters, with only base arithmetic types,
         * in a flat arrangement which can be easily used by Cython.
         */

        // System parameters
        double temperature = 0.8;
        double density = 1.0;
        int particle_count = 100;

        // Random seed
        unsigned int random_seed =
            engine::InitialCondition::random_number_engine_type::default_seed;
        
        // Force parameters
        double cutoff_distance = 2.5;

        // Time step size
        double time_delta = 0.005;

        // We use a standard sequence of one Equilibration Phase followed by one Observation Phase
        // Equilibration Phase parameters:
        std::string equilibration_phase_name = "Equilibration Phase";
        double equilibration_tolerance = 0.05;
        int equilibration_sample_size = 50;
        int equilibration_adjustment_interval = 200;
        int equilibration_steady_state_time = 1000;
        int equilibration_timeout = 5000;

        // Observation Phase parameters:
        std::string observation_phase_name = "Observation Phase";
        double observation_tolerance = 0.10;
        int observation_sample_size = 50;
        int observation_interval = 200;
        int observation_count = 20;

        // Output filenames:
        std::string event_log_path = "events.log";
        std::string thermodynamic_log_path = "thermodynamics.csv";
        std::string observation_log_path = "observations.csv";
        std::string snapshot_log_path = "snapshots.csv";
    };

    /**
     * We also provide a function which constructs the Simulation::Parameters from a set of
     * SimplifiedParameters:
     */

    Simulation::Parameters make_simulation_parameters(const SimplifiedParameters&);
} // namespace api


#endif
