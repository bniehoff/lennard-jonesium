/**
 * Test functionality of different Sinks.
 */

#include <filesystem>
#include <fstream>
#include <sstream>

#include <catch2/catch.hpp>

#include <lennardjonesium/physics/measurements.hpp>
#include <lennardjonesium/physics/observation.hpp>
#include <src/lennardjonesium/output/log_message.hpp>
#include <src/lennardjonesium/output/sinks.hpp>

SCENARIO("Testing Sink concept")
{
    using namespace output;

    constexpr bool event_sink_check = Sink<
        EventSink,
        PhaseStartEvent,
        AdjustTemperatureEvent,
        RecordObservationEvent,
        PhaseCompleteEvent,
        AbortSimulationEvent
    >;

    constexpr bool thermodynamic_sink_check = Sink<
        ThermodynamicSink,
        ThermodynamicData
    >;

    constexpr bool observation_sink_check = Sink<
        ObservationSink,
        ObservationData
    >;

    REQUIRE(event_sink_check);
    REQUIRE(thermodynamic_sink_check);
    REQUIRE(observation_sink_check);
}

SCENARIO("Sinks write correct output to files")
{
    namespace fs = std::filesystem;

    fs::path test_dir{"test_sinks"};
    fs::create_directory(test_dir);

    /**
     * We use floating point numbers which should have exact representations, in order to make it
     * more obvious what string output to expect.
     */

    GIVEN("An Eventsink has written a file")
    {
        fs::path event_log_path = test_dir / "events.txt";
        std::ofstream event_log{event_log_path};
        output::EventSink event_sink{event_log};

        std::string phase_name{"Test Phase"};
        double target_temperature{0.5};
        std::string abort_reason{"Could not reverse the polarity"};

        event_sink.write_header();
        event_sink.write(0, output::PhaseStartEvent{phase_name});
        event_sink.write(3, output::AdjustTemperatureEvent{target_temperature});
        event_sink.write(5, output::PhaseCompleteEvent{phase_name});
        event_sink.write(6, output::RecordObservationEvent{});
        event_sink.write(8, output::AbortSimulationEvent{abort_reason});

        event_log.close();

        WHEN("I read the file back in")
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
    }

    GIVEN("A ThermodynamicSink has written a file")
    {
        fs::path thermodynamic_log_path = test_dir / "thermodynamics.csv";
        std::ofstream thermodynamic_log{thermodynamic_log_path};
        output::ThermodynamicSink thermodynamic_sink{thermodynamic_log};

        physics::ThermodynamicMeasurement::Result thermodynamic_result{
            .time = 3.5,
            .kinetic_energy = 2.25,
            .potential_energy = 4.25,
            .total_energy = 6.5,
            .virial = 5.5,
            .temperature = 0.5,
            .mean_square_displacement = 7.25
        };

        thermodynamic_sink.write_header();
        thermodynamic_sink.write(7, output::ThermodynamicData{thermodynamic_result});

        thermodynamic_log.close();

        WHEN("I read the file back in")
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
    }

    GIVEN("An ObservationSink has written a file")
    {
        fs::path observation_log_path = test_dir / "observations.csv";
        std::ofstream observation_log{observation_log_path};
        output::ObservationSink observation_sink{observation_log};

        physics::Observation observation{
            .temperature = 0.5,
            .pressure = 3.25,
            .specific_heat = 2.5,
            .diffusion_coefficient = 5.25
        };

        observation_sink.write_header();
        observation_sink.write(3, output::ObservationData{observation});

        observation_log.close();

        WHEN("I read the file back in")
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
