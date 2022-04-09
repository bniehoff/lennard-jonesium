/**
 * Test the Simulation class
 */

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>
#include <memory>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/engine/force_calculation.hpp>
#include <src/lennardjonesium/engine/boundary_condition.hpp>
#include <src/lennardjonesium/engine/initial_condition.hpp>
#include <src/lennardjonesium/engine/integrator.hpp>
#include <src/lennardjonesium/output/logger.hpp>
#include <src/lennardjonesium/control/simulation_phase.hpp>
#include <src/lennardjonesium/control/simulation.hpp>

#include <tests/mock/fixed_phases.hpp>

SCENARIO("Simulation correctly interprets commands")
{
    // First set up the directory for writing simulation data files
    namespace fs = std::filesystem;

    fs::path test_dir{"test_simulation"};
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

    // Set up integrator options with no forces
    double time_delta = 0.25;

    GIVEN("A two-phase Simulation run with mock phases")
    {
        // Set up files for logging
        fs::path event_log_path = test_dir / "events.txt";
        std::ofstream event_log{event_log_path};

        fs::path thermodynamic_log_path = test_dir / "thermodynamics.csv";
        std::ofstream thermodynamic_log{thermodynamic_log_path};
        
        fs::path observation_log_path = test_dir / "observations.csv";
        std::ofstream observation_log{observation_log_path};

        // Set up the logger
        output::Logger logger{event_log, thermodynamic_log, observation_log};

        // Set up the Simulation
        control::Simulation::Schedule schedule;
        schedule.push(std::make_unique<mock::SuccessPhase>("SuccessPhase"));
        schedule.push(std::make_unique<mock::FailurePhase>("FailurePhase"));

        auto integrator = engine::VelocityVerletIntegrator(
            time_delta,
            std::make_unique<engine::PeriodicBoundaryCondition>(initial_condition.bounding_box()),
            std::make_unique<engine::NullForceCalculation>()
        );

        control::Simulation simulation(integrator, std::move(schedule), logger);

        // Run the simulation
        physics::SystemState state = initial_condition.system_state();

        state | simulation;

        // Close the logger
        logger.close();

        WHEN("I read the events log back in")
        {
            std::ifstream fin{event_log_path};
            std::ostringstream contents;

            contents << fin.rdbuf();

            THEN("I get the expected contents")
            {
                std::ostringstream expected;
                expected
                    << "0: Phase started: SuccessPhase\n"
                    << "1: Temperature adjusted to: 0.5\n"
                    << "3: Temperature adjusted to: 0.5\n"
                    << "5: Phase complete: SuccessPhase\n"
                    << "5: Phase started: FailurePhase\n"
                    << "6: Observation recorded\n"
                    << "8: Observation recorded\n"
                    << "10: Simulation aborted: Task failed successfully\n";
                
                REQUIRE(expected.view() == contents.view());
            }
        }
    }

    // Clean up
    fs::remove_all(test_dir);
}
