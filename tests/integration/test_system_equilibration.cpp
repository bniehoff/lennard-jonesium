/**
 * Test Simulation.  This is more of an integration test.  We should have enough of the pieces
 * put together to run a complete simulation.
 */

#include <filesystem>
#include <fstream>
#include <sstream>
#include <ranges>
#include <utility>
#include <memory>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/math.hpp>
#include <src/lennardjonesium/tools/moving_sample.hpp>
#include <src/lennardjonesium/tools/bounding_box.hpp>
#include <src/lennardjonesium/tools/system_parameters.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/measurements.hpp>
#include <src/lennardjonesium/engine/initial_condition.hpp>
#include <src/lennardjonesium/engine/integrator.hpp>
#include <src/lennardjonesium/engine/integrator_builder.hpp>
#include <src/lennardjonesium/output/logger.hpp>
#include <src/lennardjonesium/control/simulation_phase.hpp>
#include <src/lennardjonesium/control/simulation_controller.hpp>

#include <tests/mock/constant_short_range_force.hpp>

double measure_temperature(physics::SystemState state, const engine::Integrator& integrator)
{
    /**
     * To understand whether equilibration was successful, we cannot just measure the instantanous
     * temperature of the final state, because this is a single microstate and might not be
     * representative of the thermodynamic temperature.  Instead we have to evolve the system
     * for some time, take temperature samples, and compute the average temperature.
     */

    int samples = 20;
    int sample_interval = 5;

    tools::MovingSample<double> temperature_samples(samples);

    for ([[maybe_unused]] int i : std::views::iota(0, samples))
    {
        state | integrator(sample_interval);
        temperature_samples.push_back(physics::temperature(state));
    }

    return temperature_samples.statistics().mean;
}


SCENARIO("Equilibrating the system")
{
    /**
     * First set up the directory for writing simulation data files
     */
    namespace fs = std::filesystem;

    fs::path test_dir{"test_system_equilibration"};
    fs::create_directory(test_dir);

    /**
     * Next create the initial condition.  We will deliberately create an initial condition with
     * the wrong temperature, so that we can test that the equilibrator rescales appropriately.
     */

    // Define system parameters
    tools::SystemParameters system_parameters{
        .temperature {0.3},
        .density {0.8},
        .particle_count {500}
    };

    // Target system parameters are different (this is not normal)
    tools::SystemParameters target_system_parameters = system_parameters;
    target_system_parameters.temperature = 0.8;

    // This always uses the default random seed, so the test is repeatable
    engine::InitialCondition initial_condition(system_parameters);

    // Now set up the IntegratorBuilder
    double force = 4.0;
    double cutoff_distance = 2.0;
    double time_delta = 0.005;

    engine::Integrator::Builder builder{time_delta};

    // Build one integrator to use for temperature measurement
    // (Since the unique_ptr is passed to the simulation, we cannot reuse the same integrator)
    // TODO: Need Simulation builder to make this more convenient
    auto measurement_force = std::make_unique<mock::ConstantShortRangeForce>(
        force, cutoff_distance
    );

    auto measurement_integrator = builder
        .bounding_box(initial_condition.bounding_box())
        .short_range_force(std::move(measurement_force))
        .build();

    GIVEN("An EquilibrationPhase with a large tolerance")
    {
        // Set up files for logging
        fs::path local_dir = test_dir / "good_run";
        fs::create_directory(local_dir);

        fs::path event_log_path = local_dir / "events.txt";
        std::ofstream event_log{event_log_path};

        fs::path thermodynamic_log_path = local_dir / "thermodynamics.csv";
        std::ofstream thermodynamic_log{thermodynamic_log_path};
        
        fs::path observation_log_path = local_dir / "observations.csv";
        std::ofstream observation_log{observation_log_path};

        // Set up the logger
        output::Logger logger{event_log, thermodynamic_log, observation_log};

        // Set up the SimulationController
        control::EquilibrationPhase::Parameters equilibration_parameters{
            .tolerance {0.10},
            .sample_size {50},
            .adjustment_interval {100},
            .steady_state_time {5000},
            .timeout {50000}
        };

        control::SimulationController::Schedule schedule;

        schedule.push(
            std::make_unique<control::EquilibrationPhase>(
                "Large-tolerance Equilibration Phase",
                target_system_parameters,
                equilibration_parameters
            )
        );

        auto short_range_force = std::make_unique<mock::ConstantShortRangeForce>(
            force, cutoff_distance
        );
        
        auto integrator = builder
            .bounding_box(initial_condition.bounding_box())
            .short_range_force(std::move(short_range_force))
            .build();

        control::SimulationController simulation(std::move(integrator), std::move(schedule), logger);

        THEN("The system equilibrates to the desired temperature")
        {
            physics::SystemState state = initial_condition.system_state();

            state | simulation;

            REQUIRE(
                tools::relative_error(
                    measure_temperature(state, *measurement_integrator),
                    target_system_parameters.temperature
                ) < equilibration_parameters.tolerance
            );
        }
    }

    GIVEN("An EquilibrationPhase with a very small tolerance")
    {
        // Set up files for logging
        fs::path local_dir = test_dir / "bad_run";
        fs::create_directory(local_dir);

        fs::path event_log_path = local_dir / "events.txt";
        std::ofstream event_log{event_log_path};

        fs::path thermodynamic_log_path = local_dir / "thermodynamics.csv";
        std::ofstream thermodynamic_log{thermodynamic_log_path};
        
        fs::path observation_log_path = local_dir / "observations.csv";
        std::ofstream observation_log{observation_log_path};

        // Set up the logger
        output::Logger logger{event_log, thermodynamic_log, observation_log};

        // Set up the SimulationController
        control::EquilibrationPhase::Parameters equilibration_parameters{
            .tolerance {0.000001},
            .sample_size {50},
            .adjustment_interval {100},
            .steady_state_time {5000},
            .timeout {20000}
        };

        control::SimulationController::Schedule schedule;

        schedule.push(
            std::make_unique<control::EquilibrationPhase>(
                "Small-tolerance Equilibration Phase",
                target_system_parameters,
                equilibration_parameters
            )
        );

        auto short_range_force = std::make_unique<mock::ConstantShortRangeForce>(
            force, cutoff_distance
        );
        
        auto integrator = builder
            .bounding_box(initial_condition.bounding_box())
            .short_range_force(std::move(short_range_force))
            .build();

        control::SimulationController simulation(std::move(integrator), std::move(schedule), logger);

        THEN("The system fails to equilibrate")
        {
            physics::SystemState state = initial_condition.system_state();

            state | simulation;

            REQUIRE(
                tools::relative_error(
                    measure_temperature(state, *measurement_integrator),
                    target_system_parameters.temperature
                ) > equilibration_parameters.tolerance
            );
        }
    }

    // Clean up
    fs::remove_all(test_dir);
}
