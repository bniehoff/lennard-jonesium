/**
 * configuration.hpp
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
#include <memory>

#include <lennardjonesium/api/seed_generator.hpp>
#include <lennardjonesium/api/simulation.hpp>

namespace api
{
    struct Configuration
    {
        /**
         * The Configuration presents a simplified view of the Simulation::Parameters which uses
         * only basic types which are easily understood by Cython.  This will make it easier to
         * create the interface between the two languages.
         * 
         * Note: These defaults are not especially important.  We will define new defaults in
         * Cython.  However, the default random seed is useful.
         */

        struct System
        {
            // System parameters
            double temperature = 0.8;
            double density = 1.0;
            int particle_count = 100;

            // Random seed
            unsigned int random_seed = SeedGenerator::default_seed;
            
            // Force parameters
            double cutoff_distance = 2.5;

            // Time step size
            double time_delta = 0.005;
        };

        struct Equilibration
        {
            // Equilibration Phase parameters:
            std::string name = "Equilibration Phase";
            double tolerance = 0.05;
            int sample_size = 50;
            int adjustment_interval = 200;
            int steady_state_time = 1000;
            int timeout = 5000;
        };

        struct Observation
        {
            // Observation Phase parameters:
            std::string name = "Observation Phase";
            double tolerance = 0.10;
            int sample_size = 50;
            int observation_interval = 200;
            int observation_count = 20;
        };

        struct Filepaths
        {
            // Output filenames:
            std::string event_log = "events.log";
            std::string thermodynamic_log = "thermodynamics.csv";
            std::string observation_loh = "observations.csv";
            std::string snapshot_log = "snapshots.csv";
        };

        /**
         * We include one instance of each parameter category.  So, we will always run a simulation
         * consisting of one Equilibration phase followed by one Observation phase.
         */

        System system{};
        Equilibration equilibration{};
        Observation observation{};
        Filepaths filepaths{};
    };

    /**
     * We also provide a factory function which creates a simulation from these parameters.
     */
    std::unique_ptr<Simulation> make_simulation(const Configuration&);
} // namespace api


#endif
