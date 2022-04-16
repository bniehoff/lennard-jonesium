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
#include <lennardjonesium/control/simulation.hpp>

namespace control
{
    Simulation::Simulation(Simulation::Parameters parameters)
        : parameters_{parameters},
          initial_condition_{
              parameters.system_parameters,
              parameters.unit_cell,
              parameters.random_seed
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

    void Simulation::run()
    {
        // First create the file streams.  We always duplicate the events log to stdout.
        namespace bio = boost::iostreams;
        using tee_device = bio::tee_device<std::ostream, bio::file_sink>;
        using tee_stream = bio::stream<tee_device>;
        using ofstream = bio::stream<bio::file_sink>;

        auto event_stream = tee_stream(
            tee_device(std::cout, bio::file_sink{parameters_.event_log_path})
        );

        auto thermodynamic_stream = ofstream(bio::file_sink{parameters_.thermodynamic_log_path});
        auto observation_stream = ofstream(bio::file_sink{parameters_.observation_log_path});

        // Create the logger
        output::Logger logger{event_stream, thermodynamic_stream, observation_stream};

        // Get the SimulationController
        auto simulation_controller = make_simulation_controller_(logger);

        // Generate the initial state and actually run the simulation
        physics::SystemState state = initial_condition_.system_state();
        state | simulation_controller;

        // Close the logger and file streams
        logger.close();

        event_stream.close();
        thermodynamic_stream.close();
        observation_stream.close();
    }

    SimulationController Simulation::make_simulation_controller_(output::Logger& logger)
    {
        // First build the integrator
        auto integrator = engine::Integrator::Builder(parameters_.time_delta)
            .bounding_box(initial_condition_.bounding_box())
            .short_range_force(*short_range_force_)
            .build();
        
        // Next assemble the scheduler
        SimulationController::Schedule schedule;

        for (auto [name, phase_parameters] : parameters_.schedule_parameters)
        {
            if (auto eq_phase_parameters =
                std::get_if<EquilibrationPhase::Parameters>(&phase_parameters))
            {
                schedule.push(
                    std::make_unique<EquilibrationPhase>(
                        name, parameters_.system_parameters, *eq_phase_parameters
                    )
                );
            }

            if (auto ob_phase_parameters =
                std::get_if<ObservationPhase::Parameters>(&phase_parameters))
            {
                schedule.push(
                    std::make_unique<ObservationPhase>(
                        name, parameters_.system_parameters, *ob_phase_parameters
                    )
                );
            }
        }

        // Finally return the SimulationController
        return {std::move(integrator), std::move(schedule), logger};
    }
} // namespace control
