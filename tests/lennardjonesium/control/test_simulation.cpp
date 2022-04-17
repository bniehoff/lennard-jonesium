/**
 * Test a complete run of a Simulation
 */

#include <filesystem>
#include <fstream>
#include <string>

#include <catch2/catch.hpp>

#include <src/lennardjonesium/physics/lennard_jones_force.hpp>
#include <src/lennardjonesium/control/simulation_phase.hpp>
#include <src/lennardjonesium/control/simulation.hpp>

namespace fs = std::filesystem;

int count_lines(fs::path file_path)
{
    std::ifstream fin{file_path};

    std::string unused;
    int count = 0;

    while (std::getline(fin, unused))
    {
        ++count;
    }

    fin.close();

    return count;
}

SCENARIO("Running a simulation from a Simulation object")
{
    // First set up the directory for writing simulation data files
    fs::path test_dir{"test_simulation"};
    fs::create_directory(test_dir);

    int observation_interval = 100;
    int observation_count = 20;

    // Next set up simulation parameters
    auto parameters = control::Simulation::Parameters
    {
        .system_parameters = {
            .temperature = 0.8,
            .density = 0.8,
            .particle_count = 50
        },

        .force_parameters = physics::LennardJonesForce::Parameters
        {
            .strength = 1.0,
            .cutoff_distance = 2.0
        },

        .time_delta = 0.005,

        // For the unit test, we do not need an equilibration phase, just check that the simulation
        // runs for the right number of time steps
        .schedule_parameters = {
            {
                "Observation Phase",
                control::ObservationPhase::Parameters
                {
                    .tolerance = 10.0,
                    .sample_size = 25,
                    .observation_interval = observation_interval,
                    .observation_count = observation_count
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
            int event_lines = observation_count + 2;
            int thermodynamic_lines = (observation_count * observation_interval) + 1;
            int observation_lines = observation_count + 1;

            REQUIRE(event_lines == count_lines(parameters.event_log_path));
            REQUIRE(thermodynamic_lines == count_lines(parameters.thermodynamic_log_path));
            REQUIRE(observation_lines == count_lines(parameters.observation_log_path));
        }
    }

    // Clean up
    fs::remove_all(test_dir);
}
