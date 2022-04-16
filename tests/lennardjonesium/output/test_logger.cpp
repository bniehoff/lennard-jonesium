/**
 * Test the Logger and verify it sends everything to the appropriate files
 */

#include <filesystem>
#include <fstream>
#include <sstream>

#include <catch2/catch.hpp>

#include <src/lennardjonesium/physics/measurements.hpp>
#include <src/lennardjonesium/physics/observation.hpp>
#include <src/lennardjonesium/output/log_message.hpp>
#include <src/lennardjonesium/output/logger.hpp>

SCENARIO("Logger sends to correct files")
{
    // We simply repeat the test from test_sinks.cpp, but all at once
    namespace fs = std::filesystem;

    fs::path test_dir{"test_logger"};
    fs::create_directory(test_dir);

    // Set up the output files
    fs::path event_log_path = test_dir / "events.log";
    std::ofstream event_log{event_log_path};

    fs::path thermodynamic_log_path = test_dir / "thermodynamics.csv";
    std::ofstream thermodynamic_log{thermodynamic_log_path};
    
    fs::path observation_log_path = test_dir / "observations.csv";
    std::ofstream observation_log{observation_log_path};

    GIVEN("The logger has been sent a number of messages")
    {
        // Set up the logger
        output::Logger logger{event_log, thermodynamic_log, observation_log};

        // Create the messages
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

        // Send the meessages
        logger.log(0, output::PhaseStartEvent{phase_name});
        logger.log(3, output::AdjustTemperatureEvent{target_temperature});
        logger.log(3, output::ObservationData{observation});
        logger.log(5, output::PhaseCompleteEvent{phase_name});
        logger.log(6, output::RecordObservationEvent{});
        logger.log(7, output::ThermodynamicData{thermodynamic_result});
        logger.log(8, output::AbortSimulationEvent{abort_reason});

        // Close the logger
        logger.close();

        // Close the files
        event_log.close();
        thermodynamic_log.close();
        observation_log.close();
        
        WHEN("I read the events log back in")
        {
            std::ifstream fin{event_log_path};
            std::ostringstream contents;

            contents << fin.rdbuf();

            THEN("I get the expected contents")
            {
                std::ostringstream expected;
                expected
                    << "0: Phase started: Test Phase\n"
                    << "3: Temperature adjusted to: 0.5\n"
                    << "5: Phase complete: Test Phase\n"
                    << "6: Observation recorded\n"
                    << "8: Simulation aborted: Could not reverse the polarity\n";
                
                REQUIRE(expected.view() == contents.view());
            }
        }

        WHEN("I read the thermodynamic log back in")
        {
            std::ifstream fin{thermodynamic_log_path};
            std::ostringstream contents;

            contents << fin.rdbuf();

            THEN("I get the expected contents")
            {
                std::ostringstream expected;
                expected
                    << "TimeStep,Time,KineticEnergy,PotentialEnergy,TotalEnergy,"
                    << "Virial,Temperature,MeanSquareDisplacement\n"
                    << "7,3.5,2.25,4.25,6.5,5.5,0.5,7.25\n";
                
                REQUIRE(expected.view() == contents.view());
            }
        }
        

        WHEN("I read the observation log back in")
        {
            std::ifstream fin{observation_log_path};
            std::ostringstream contents;

            contents << fin.rdbuf();

            THEN("I get the expected file contents")
            {
                std::ostringstream expected;
                expected
                    << "TimeStep,Temperature,Pressure,SpecificHeat,DiffusionCoefficient\n"
                    << "3,0.5,3.25,2.5,5.25\n";
                
                REQUIRE(expected.view() == contents.view());
            }
        }
    }

    // Clean up
    fs::remove_all(test_dir);
}
