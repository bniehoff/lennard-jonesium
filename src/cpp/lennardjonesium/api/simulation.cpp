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
#include <mutex>
#include <exception>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/chain.hpp>
#include <boost/iostreams/device/file.hpp>

#include <lennardjonesium/tools/overloaded_visitor.hpp>
#include <lennardjonesium/tools/system_parameters.hpp>
#include <lennardjonesium/tools/text_buffer.hpp>
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

    std::shared_ptr<tools::TextBuffer> Simulation::launch(Simulation::EchoMode echo_mode)
    {
        // Wait for any currently-running jobs to finish
        wait();

        // Start with a nullptr; we will create a buffer if requested
        std::shared_ptr<tools::TextBuffer> buffer;

        // Configure the echo chain
        echo_chain_type echo_chain{};

        switch (echo_mode)
        {
        case EchoMode::console:
            echo_chain.push(boost::iostreams::tee(std::cout));
            break;
        
        case EchoMode::buffer:
            buffer = std::make_shared<tools::TextBuffer>();
            echo_chain.push(tools::TextBufferFilter(*buffer));
            break;
        
        case EchoMode::silent:
            [[fallthrough]];
        default:
            break;
        }

        using event_stream_type = boost::iostreams::filtering_ostream;
        using file_sink_type = boost::iostreams::file_sink;
        using file_stream_type = boost::iostreams::stream<file_sink_type>;

        // Launch the simulation job (it sets up its own local variables)
        simulation_job_ = std::jthread(
            [this, echo_chain, buffer]() mutable
            {
                // Set up streams
                echo_chain.push(file_sink_type{this->parameters_.event_log_path});
                event_stream_type event_stream{echo_chain};

                file_stream_type thermodynamic_stream{
                    file_sink_type{this->parameters_.thermodynamic_log_path}
                };

                file_stream_type observation_stream{
                    file_sink_type{this->parameters_.observation_log_path}
                };

                file_stream_type snapshot_stream{
                    file_sink_type{this->parameters_.snapshot_log_path}
                };

                // Set up logger
                output::Logger logger{output::Logger::Streams{
                    .event_log = event_stream,
                    .thermodynamic_log = thermodynamic_stream,
                    .observation_log = observation_stream,
                    .snapshot_log = snapshot_stream
                }};
                
                // Create initial state and SimulationController
                auto initial_state = this->initial_condition_.system_state();
                auto simulation_controller = make_simulation_controller_(logger);

                // Run the actual simulation
                initial_state | simulation_controller;

                // Close the logger
                logger.close();

                // Close the streams (note that event_stream is a different type)
                event_stream.reset();
                thermodynamic_stream.close();
                observation_stream.close();
                snapshot_stream.close();

                // Free our copy of the shared buffer
                buffer.reset();
            }
        );

        // Return the shared buffer to the caller
        return buffer;
    }

    void Simulation::wait()
    {
        // If a simulation job is running, then wait for it to end
        if (simulation_job_.joinable())
        {
            simulation_job_.join();
        }
    }

    // run() is just a synchronous wrapper for launch() and wait()
    void Simulation::run(EchoMode echo_mode)
    {
        // There is no point in using buffered echo mode
        if (echo_mode == EchoMode::buffer) {echo_mode = EchoMode::silent;}

        launch(echo_mode);
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
