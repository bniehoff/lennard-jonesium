/**
 * configuration.cpp
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

#include <variant>
#include <random>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>

#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/tools/cubic_lattice.hpp>
#include <lennardjonesium/physics/lennard_jones_force.hpp>
#include <lennardjonesium/engine/initial_condition.hpp>
#include <lennardjonesium/control/simulation_phase.hpp>
#include <lennardjonesium/api/simulation.hpp>
#include <lennardjonesium/api/configuration.hpp>

namespace api
{
    std::unique_ptr<Simulation> make_simulation(const Configuration& configuration)
    {
        // First create the Simulation::Parameters struct
        Simulation::Parameters parameters
        {
            .system_parameters = {
                .temperature = configuration.system.temperature,
                .density = configuration.system.density,
                .particle_count = configuration.system.particle_count
            },

            .unit_cell = tools::CubicLattice::FaceCentered(),

            .random_seed = configuration.system.random_seed,

            .force_parameters = physics::LennardJonesForce::Parameters{
                .cutoff_distance = configuration.system.cutoff_distance
            },

            .time_delta = configuration.system.time_delta,

            .schedule_parameters = {
                {
                    configuration.equilibration.name,
                    control::EquilibrationPhase::Parameters{
                        .tolerance = configuration.equilibration.tolerance,
                        .sample_size =  configuration.equilibration.sample_size,
                        .adjustment_interval = configuration.equilibration.adjustment_interval,
                        .steady_state_time = configuration.equilibration.steady_state_time,
                        .timeout = configuration.equilibration.timeout
                    }
                },

                {
                    configuration.observation.name,
                    control::ObservationPhase::Parameters{
                        .tolerance = configuration.observation.tolerance,
                        .sample_size = configuration.observation.sample_size,
                        .observation_interval = configuration.observation.observation_interval,
                        .observation_count = configuration.observation.observation_count
                    }
                }
            },

            .event_log_path = configuration.filepaths.event_log,
            .thermodynamic_log_path = configuration.filepaths.thermodynamic_log,
            .observation_log_path = configuration.filepaths.observation_loh,
            .snapshot_log_path = configuration.filepaths.snapshot_log
        };

        // Now create the Simulation object
        return std::make_unique<Simulation>(parameters);
    }
} // namespace api

