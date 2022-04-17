// Test a full run of the simulation using Simulation::run()

#include <filesystem>

#include <catch2/catch.hpp>

#include <src/lennardjonesium/physics/lennard_jones_force.hpp>
#include <src/lennardjonesium/control/simulation_phase.hpp>
#include <src/lennardjonesium/control/simulation.hpp>

namespace fs = std::filesystem;

SCENARIO("A complete run of the simulation")
{
    // First set up the directory for writing simulation data files
    fs::path test_dir{"test_full_simulation"};
    fs::create_directory(test_dir);

    // Next set up simulation parameters
    auto parameters = control::Simulation::Parameters
    {
        .system_parameters = {
            .temperature = 0.66,
            .density = 0.81,
            .particle_count = 200
        },

        .force_parameters = physics::LennardJonesForce::Parameters
        {
            .cutoff_distance = 2.0
        },

        .time_delta = 0.005,

        // For the unit test, we do not need an equilibration phase, just check that the simulation
        // runs for the right number of time steps
        .schedule_parameters = {
            {
                "Equilibration Phase",
                control::EquilibrationPhase::Parameters
                {
                    .tolerance = 0.02,
                    .sample_size = 250,
                    .adjustment_interval = 1000,
                    .steady_state_time = 5000,
                    .timeout = 100000
                }
            },
            {
                "Observation Phase",
                control::ObservationPhase::Parameters
                {
                    .tolerance = 0.10,
                    .sample_size = 100,
                    .observation_interval = 200,
                    .observation_count = 50
                }
            }
        },

        .event_log_path = test_dir / "events.log",
        .thermodynamic_log_path = test_dir / "thermodynamics.csv",
        .observation_log_path = test_dir / "observations.csv"
    };

    // Finally, create the Simulation object
    control::Simulation simulation{parameters};

    WHEN("I run the simulation")
    {
        simulation.run();

        THEN("I get the expected output files")
        {
           //
        }
    }

    // Clean up
    fs::remove_all(test_dir);
}
