/**
 * Test a complete run of a Simulation
 */

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <ranges>

#include <catch2/catch.hpp>

#include <src/cpp/lennardjonesium/physics/lennard_jones_force.hpp>
#include <src/cpp/lennardjonesium/control/simulation_phase.hpp>
#include <src/cpp/lennardjonesium/api/simulation.hpp>
#include <src/cpp/lennardjonesium/api/simulation_pool.hpp>

namespace fs = std::filesystem;

inline int count_lines(fs::path file_path)
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

SCENARIO("Running a pool of simulations")
{
    // First set up the directory for writing simulation data files
    fs::path test_dir{"test_simulation_pool"};
    fs::create_directory(test_dir);

    int observation_interval = 100;
    int observation_count = 20;
    int job_count = 12;
    int thread_count = 4;

    // Next set up the global simulation parameters
    auto parameters = api::Simulation::Parameters
    {
        .system_parameters = {
            .temperature = 0.8,
            .density = 0.8,
            .particle_count = 50
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
                "Observation Phase",
                control::ObservationPhase::Parameters
                {
                    .tolerance = 10.0,
                    .sample_size = 25,
                    .observation_interval = observation_interval,
                    .observation_count = observation_count
                }
            }
        }
    };

    // Filenames
    fs::path event_log_path = "events.log";
    fs::path thermodynamic_log_path = "thermodynamics.csv";
    fs::path observation_log_path = "observations.csv";
    fs::path snapshot_log_path = "snapshots.csv";

    // Now prepare Simulations with multiple different subdirectories for output
    std::vector<api::Simulation> simulations;
    for (auto i : std::views::iota(0, job_count))
    {
        fs::path subdirectory = test_dir / std::to_string(i);
        fs::create_directory(subdirectory);

        parameters.event_log_path = subdirectory / event_log_path;
        parameters.thermodynamic_log_path = subdirectory / thermodynamic_log_path;
        parameters.observation_log_path = subdirectory / observation_log_path;
        parameters.snapshot_log_path = subdirectory / snapshot_log_path;

        simulations.emplace_back(parameters);
    }

    // Finally, create the SimulationPool
    api::SimulationPool simulation_pool(thread_count);

    WHEN("I push all the jobs onto the queue and wait for them to finish")
    {
        for (auto& s : simulations)
        {
            simulation_pool.push(s);
        }

        simulation_pool.wait();

        THEN("I get the expected output files")
        {
            int event_lines = observation_count + 2;
            int thermodynamic_lines = (observation_count * observation_interval) + 1;
            int observation_lines = observation_count + 1;

            for (auto& s : simulations)
            {
                REQUIRE(event_lines == count_lines(s.parameters().event_log_path));
                REQUIRE(thermodynamic_lines == count_lines(s.parameters().thermodynamic_log_path));
                REQUIRE(observation_lines == count_lines(s.parameters().observation_log_path));
            }
        }
    }

    // Clean up
    fs::remove_all(test_dir);
}
