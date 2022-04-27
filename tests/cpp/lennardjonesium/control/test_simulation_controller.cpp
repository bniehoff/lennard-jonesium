/**
 * Test the SimulationController class
 */

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>
#include <memory>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/cpp/lennardjonesium/engine/force_calculation.hpp>
#include <src/cpp/lennardjonesium/engine/boundary_condition.hpp>
#include <src/cpp/lennardjonesium/engine/initial_condition.hpp>
#include <src/cpp/lennardjonesium/engine/integrator.hpp>
#include <src/cpp/lennardjonesium/engine/integrator_builder.hpp>
#include <src/cpp/lennardjonesium/output/logger.hpp>
#include <src/cpp/lennardjonesium/control/simulation_phase.hpp>
#include <src/cpp/lennardjonesium/control/simulation_controller.hpp>

#include <tests/cpp/mock/fixed_phases.hpp>

SCENARIO("SimulationController correctly interprets commands")
{
    // First set up the directory for writing simulation data files
    namespace fs = std::filesystem;

    fs::path test_dir{"test_simulation_controller"};
    fs::create_directory(test_dir);

    // Next set up the initial condition
    // Define system parameters
    tools::SystemParameters system_parameters{
        .temperature {1.5},
        .density {1.0},
        .particle_count {4}
    };

    // This always uses the default random seed, so the test is repeatable
    engine::InitialCondition initial_condition(system_parameters);

    // Set up integrator builder with time delta that prints nicely
    double time_delta = 0.25;
    engine::Integrator::Builder builder{time_delta};

    GIVEN("A two-phase SimulationController run with mock phases")
    {
        // Set up files for logging
        fs::path event_log_path = test_dir / "events.log";
        std::ofstream event_log{event_log_path};

        fs::path thermodynamic_log_path = test_dir / "thermodynamics.csv";
        std::ofstream thermodynamic_log{thermodynamic_log_path};
        
        fs::path observation_log_path = test_dir / "observations.csv";
        std::ofstream observation_log{observation_log_path};
    
        fs::path snapshot_log_path = test_dir / "snapshots.csv";
        std::ofstream snapshot_log{snapshot_log_path};

        output::Logger::Streams streams = {
            .event_log = event_log,
            .thermodynamic_log = thermodynamic_log,
            .observation_log = observation_log,
            .snapshot_log = snapshot_log
        };

        // Set up the logger
        output::Logger logger{streams};

        // Set up the SimulationController
        control::SimulationController::Schedule schedule;
        schedule.push(std::make_unique<mock::SuccessPhase>("SuccessPhase"));
        schedule.push(std::make_unique<mock::FailurePhase>("FailurePhase"));

        // Set up integrator with no forces
        auto integrator = builder.bounding_box(initial_condition.bounding_box()).build();

        // Create simulation
        control::SimulationController simulation(std::move(integrator), std::move(schedule), logger);

        // Run the simulation
        physics::SystemState state = initial_condition.system_state();

        state | simulation;

        // Close the logger
        logger.close();

        // Close the files
        event_log.close();
        thermodynamic_log.close();
        observation_log.close();
        snapshot_log.close();

        WHEN("I read the events log back in")
        {
            std::ifstream fin{event_log_path};
            std::ostringstream contents;

            contents << fin.rdbuf();

            THEN("I get the expected contents")
            {
                std::string expected = 
                    "0: Phase started: SuccessPhase\n"
                    "1: Temperature measured at: 0.25, adjusted to: 0.5\n"
                    "3: Temperature measured at: 0.25, adjusted to: 0.5\n"
                    "5: Phase complete: SuccessPhase\n"
                    "5: Phase started: FailurePhase\n"
                    "6: Observation recorded\n"
                    "8: Observation recorded\n"
                    "10: Simulation aborted: Task failed successfully\n";
                
                REQUIRE(expected == contents.view());
            }
        }
    }

    // Clean up
    fs::remove_all(test_dir);
}
