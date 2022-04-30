/**
 * simulation.cpp
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

#include <cassert>
#include <memory>
#include <variant>
#include <vector>
#include <utility>
#include <filesystem>
#include <iostream>
#include <thread>
#include <exception>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>

#include <lennardjonesium/tools/overloaded_visitor.hpp>
#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/physics/forces.hpp>
#include <lennardjonesium/physics/lennard_jones_force.hpp>
#include <lennardjonesium/engine/initial_condition.hpp>
#include <lennardjonesium/engine/integrator_builder.hpp>
#include <lennardjonesium/control/simulation_phase.hpp>
#include <lennardjonesium/control/simulation_controller.hpp>
#include <lennardjonesium/api/simulation.hpp>

namespace api
{
    Simulation::Simulation(Simulation::Parameters parameters)
        : parameters_{parameters},
          initial_condition_{
              parameters.system_parameters,
              parameters.random_seed,
              parameters.unit_cell
          }
    {
        // We need to create the ShortRangeForce
        auto short_range_force_constructor = tools::OverloadedVisitor
        {
            [](physics::LennardJonesForce::Parameters lj_parameters)
                -> std::unique_ptr<physics::LennardJonesForce>
            {return std::make_unique<physics::LennardJonesForce>(lj_parameters);}
        };

        short_range_force_ = std::visit(
            short_range_force_constructor,
            parameters.force_parameters
        );

        assert(short_range_force_ != nullptr && "Failed to construct ShortRangeForce");
    }

    void Simulation::launch(std::ostream& echo_stream)
    {
        // Wait for any currently-running jobs to finish
        wait();

        // Next open the file streams.  We duplicate the events log to the given echo stream.
        namespace bio = boost::iostreams;

        event_stream_.open(bio::tee(echo_stream, bio::file_sink{parameters_.event_log_path}));
        thermodynamic_stream_.open(bio::file_sink{parameters_.thermodynamic_log_path});
        observation_stream_.open(bio::file_sink{parameters_.observation_log_path});
        snapshot_stream_.open(bio::file_sink{parameters_.snapshot_log_path});

        // Recreate the Logger
        logger_ = std::make_unique<output::Logger>(output::Logger::Streams{
            .event_log = event_stream_,
            .thermodynamic_log = thermodynamic_stream_,
            .observation_log = observation_stream_,
            .snapshot_log = snapshot_stream_
        });

        // Create the SimulationController and initial state
        auto simulation_controller = make_simulation_controller_(*logger_);
        auto initial_state = initial_condition_.system_state();

        // Launch the simulation job
        simulation_job_ = std::jthread(
            [simulation_controller=std::move(simulation_controller), initial_state]() mutable
            {
                initial_state | simulation_controller;
            }
        );
    }

    void Simulation::wait()
    {
        // If a simulation job is running, then wait for it to end and then clean up
        if (simulation_job_.joinable())
        {
            simulation_job_.join();

            // Destroy the Logger (waits for logging to finish)
            logger_.reset();

            // Close the file streams
            event_stream_.close();
            thermodynamic_stream_.close();
            observation_stream_.close();
            snapshot_stream_.close();
        }
    }

    void Simulation::run(std::ostream& echo_stream)
    {
        // run() is just a synchronous wrapper for launch() and wait()
        launch(echo_stream);
        wait();
    }

    Simulation::~Simulation()
    {
        // Wait for any running simulations to finish
        try {wait();}
        catch(...) {}
    }

    control::SimulationController Simulation::make_simulation_controller_(output::Logger& logger)
    {
        // First build the integrator
        auto integrator = engine::Integrator::Builder(parameters_.time_delta)
            .bounding_box(initial_condition_.bounding_box())
            .short_range_force(*short_range_force_)
            .build();
        
        // Next assemble the scheduler
        control::SimulationController::Schedule schedule;

        for (auto [name, phase_parameters] : parameters_.schedule_parameters)
        {
            if (auto eq_phase_parameters =
                std::get_if<control::EquilibrationPhase::Parameters>(&phase_parameters))
            {
                schedule.push(
                    std::make_unique<control::EquilibrationPhase>(
                        name, parameters_.system_parameters, *eq_phase_parameters
                    )
                );
            }

            if (auto ob_phase_parameters =
                std::get_if<control::ObservationPhase::Parameters>(&phase_parameters))
            {
                schedule.push(
                    std::make_unique<control::ObservationPhase>(
                        name, parameters_.system_parameters, *ob_phase_parameters
                    )
                );
            }
        }

        // Finally return the SimulationController
        return {std::move(integrator), std::move(schedule), logger};
    }
} // namespace api
