/**
 * Test the Dispatcher to verify it sends to the appropriate destinations
 */

#include <filesystem>
#include <fstream>
#include <sstream>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/cpp/lennardjonesium/physics/measurements.hpp>
#include <src/cpp/lennardjonesium/physics/observation.hpp>
#include <src/cpp/lennardjonesium/output/log_message.hpp>
#include <src/cpp/lennardjonesium/output/sinks.hpp>
#include <src/cpp/lennardjonesium/output/dispatcher.hpp>

SCENARIO("Dispatcher sends to correct files")
{
    // We simply repeat the test from test_sinks.cpp, but all at once
    namespace fs = std::filesystem;

    fs::path test_dir{"test_dispatcher"};
    fs::create_directory(test_dir);

    // Set up the sinks
    fs::path event_log_path = test_dir / "events.log";
    std::ofstream event_log{event_log_path};
    output::EventSink event_sink{event_log};

    fs::path thermodynamic_log_path = test_dir / "thermodynamics.csv";
    std::ofstream thermodynamic_log{thermodynamic_log_path};
    output::ThermodynamicSink thermodynamic_sink{thermodynamic_log};
    
    fs::path observation_log_path = test_dir / "observations.csv";
    std::ofstream observation_log{observation_log_path};
    output::ObservationSink observation_sink{observation_log};
    
    fs::path snapshot_log_path = test_dir / "snapshots.csv";
    std::ofstream snapshot_log{snapshot_log_path};
    output::SystemSnapshotSink snapshot_sink{snapshot_log};

    event_sink.write_header();
    thermodynamic_sink.write_header();
    observation_sink.write_header();
    snapshot_sink.write_header();

    // Set up the dispatcher
    output::Dispatcher dispatcher{event_sink, thermodynamic_sink, observation_sink, snapshot_sink};

    GIVEN("The dispatcher has been sent a number of messages")
    {
        std::string phase_name{"Test Phase"};
        double target_temperature{0.5};
        std::string abort_reason{"Could not reverse the polarity"};

        physics::ThermodynamicMeasurement::Result thermodynamic_result{
            .time = 3.5,
            .kinetic_energy = 2.25,
            .potential_energy = 4.25,
            .total_energy = 6.5,
            .virial = 5.5,
            .temperature = 0.5,
            .mean_square_displacement = 7.25
        };

        physics::Observation observation{
            .temperature = 0.5,
            .pressure = 3.25,
            .specific_heat = 2.5,
            .diffusion_coefficient = 5.25
        };

        output::SystemSnapshot snapshot{
            .positions = Eigen::MatrixX4d{
                {0, 1, 2, 0}, {3, 4, 5, 0}, {6, 7, 8, 0}
            }.transpose(),

            .velocities = Eigen::MatrixX4d{
                {3, 2, 1, 0}, {6, 5, 4, 0}, {9, 8, 7, 0}
            }.transpose(),

            .forces = Eigen::MatrixX4d{
                {2, 0, 0, 0}, {0, 4, 0, 0}, {0, 0, 1, 0}
            }.transpose()
        };

        dispatcher.send(0, output::PhaseStartEvent{phase_name});
        dispatcher.send(3, output::AdjustTemperatureEvent{target_temperature});
        dispatcher.send(3, output::ObservationData{observation});
        dispatcher.send(5, output::PhaseCompleteEvent{phase_name});
        dispatcher.send(6, output::RecordObservationEvent{});
        dispatcher.send(7, output::ThermodynamicData{thermodynamic_result});
        dispatcher.send(8, output::AbortSimulationEvent{abort_reason});
        dispatcher.send(9, snapshot);

        dispatcher.flush_all();

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
                    "0: Phase started: Test Phase\n"
                    "3: Temperature adjusted to: 0.5\n"
                    "5: Phase complete: Test Phase\n"
                    "6: Observation recorded\n"
                    "8: Simulation aborted: Could not reverse the polarity\n";
                
                REQUIRE(expected == contents.view());
            }
        }

        WHEN("I read the thermodynamic log back in")
        {
            std::ifstream fin{thermodynamic_log_path};
            std::ostringstream contents;

            contents << fin.rdbuf();

            THEN("I get the expected contents")
            {
                std::string expected = 
                    "TimeStep,Time,KineticEnergy,PotentialEnergy,TotalEnergy,"
                    "Virial,Temperature,MeanSquareDisplacement\n"
                    "7,3.5,2.25,4.25,6.5,5.5,0.5,7.25\n";
                
                REQUIRE(expected == contents.view());
            }
        }

        WHEN("I read the observation log back in")
        {
            std::ifstream fin{observation_log_path};
            std::ostringstream contents;

            contents << fin.rdbuf();

            THEN("I get the expected file contents")
            {
                std::string expected = 
                    "TimeStep,Temperature,Pressure,SpecificHeat,DiffusionCoefficient\n"
                    "3,0.5,3.25,2.5,5.25\n";
                
                REQUIRE(expected == contents.view());
            }
        }

        WHEN("I read the snapshot log back in")
        {
            std::ifstream fin{snapshot_log_path};
            std::ostringstream contents;

            contents << fin.rdbuf();

            THEN("I get the expected file contents")
            {
                std::string expected = 
                    "TimeStep,ParticleID,Position,Position,Position,"
                    "Velocity,Velocity,Velocity,Force,Force,Force\n"
                    "TimeStep,ParticleID,X,Y,Z,X,Y,Z,X,Y,Z\n"
                    "9,0,0,1,2,3,2,1,2,0,0\n"
                    "9,1,3,4,5,6,5,4,0,4,0\n"
                    "9,2,6,7,8,9,8,7,0,0,1\n";
                
                REQUIRE(expected == contents.view());
            }
        }
    }

    // Clean up
    fs::remove_all(test_dir);
}
