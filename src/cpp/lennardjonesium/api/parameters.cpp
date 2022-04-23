/**
 * parameters.cpp
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

#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/tools/cubic_lattice.hpp>
#include <lennardjonesium/physics/lennard_jones_force.hpp>
#include <lennardjonesium/engine/initial_condition.hpp>
#include <lennardjonesium/control/simulation_phase.hpp>
#include <lennardjonesium/api/simulation.hpp>
#include <lennardjonesium/api/parameters.hpp>

namespace api
{
    Simulation::Parameters make_simulation_parameters(const SimplifiedParameters& sp)
    {
        return {
            .system_parameters = {
                .temperature = sp.temperature,
                .density = sp.density,
                .particle_count = sp.particle_count
            },

            .unit_cell = tools::CubicLattice::FaceCentered(),

            .random_seed = sp.random_seed,

            .force_parameters = physics::LennardJonesForce::Parameters{
                .cutoff_distance = sp.cutoff_distance
            },

            .time_delta = sp.time_delta,

            .schedule_parameters = {
                {
                    sp.equilibration_phase_name,
                    control::EquilibrationPhase::Parameters{
                        .tolerance = sp.equilibration_tolerance,
                        .sample_size =  sp.equilibration_sample_size,
                        .adjustment_interval = sp.equilibration_adjustment_interval,
                        .steady_state_time = sp.equilibration_steady_state_time,
                        .timeout = sp.equilibration_timeout
                    }
                },

                {
                    sp.observation_phase_name,
                    control::ObservationPhase::Parameters{
                        .tolerance = sp.observation_tolerance,
                        .sample_size = sp.observation_sample_size,
                        .observation_interval = sp.observation_interval,
                        .observation_count = sp.observation_count
                    }
                }
            },

            .event_log_path = sp.event_log_path,
            .thermodynamic_log_path = sp.thermodynamic_log_path,
            .observation_log_path = sp.observation_log_path,
            .snapshot_log_path = sp.snapshot_log_path
        };
    }
} // namespace api

