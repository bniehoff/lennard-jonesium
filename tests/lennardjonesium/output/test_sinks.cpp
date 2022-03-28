/**
 * Test functionality of different Sinks.
 */

#include <sstream>
#include <string>

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

SCENARIO("Sinks produce correct output")
{
    // Create some mock output files
    std::ostringstream event_log;
    std::ostringstream thermodynamic_log;
    std::ostringstream observation_log;

    /**
     * Note: We use floating point numbers which should have exact representations, in order to
     * make it more obvious what string output to expect.
     */

    GIVEN("An EventSink")
    {
        output::EventSink event_sink{event_log};

        std::string phase_name{"Test Phase"};
        double target_temperature{0.5};
        std::string abort_reason{"Could not reverse the polarity"};

        WHEN("I send various Event messages")
        {
            event_sink.write_header();
            event_sink.write(0, output::PhaseStartEvent{phase_name});
            event_sink.write(3, output::AdjustTemperatureEvent{target_temperature});
            event_sink.write(5, output::PhaseCompleteEvent{phase_name});
            event_sink.write(6, output::RecordObservationEvent{});
            event_sink.write(8, output::AbortSimulationEvent{abort_reason});

            THEN("I get the expected file contents")
            {
                std::ostringstream expected;
                expected
                    << "0: Phase started: Test Phase\n"
                    << "3: Temperature adjusted to: 0.5\n"
                    << "5: Phase complete: Test Phase\n"
                    << "6: Observation recorded\n"
                    << "8: Simulation aborted: Could not reverse the polarity\n";
                
                REQUIRE(expected.view() == event_log.view());
            }
        }
    }

    GIVEN("A ThermodynamicSink")
    {
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

        WHEN("I send a ThermodynamicData message")
        {
            thermodynamic_sink.write_header();
            thermodynamic_sink.write(7, output::ThermodynamicData{thermodynamic_result});

            THEN("I get the expacted file contents")
            {
                std::ostringstream expected;
                expected
                    << "TimeStep,Time,KineticEnergy,PotentialEnergy,TotalEnergy,"
                    << "Virial,Temperature,MeanSquareDisplacement\n"
                    << "7,3.5,2.25,4.25,6.5,5.5,0.5,7.25\n";
                
                REQUIRE(expected.view() == thermodynamic_log.view());
            }
        }
    }

    GIVEN("An ObservationSink")
    {
        output::ObservationSink observation_sink{observation_log};

        physics::Observation observation{
            .temperature = 0.5,
            .pressure = 3.25,
            .specific_heat = 2.5,
            .diffusion_coefficient = 5.25
        };

        WHEN("I send an ObservationData message")
        {
            observation_sink.write_header();
            observation_sink.write(3, output::ObservationData{observation});

            THEN("I get the expected file contents")
            {
                std::ostringstream expected;
                expected
                    << "TimeStep,Temperature,Pressure,SpecificHeat,DiffusionCoefficient\n"
                    << "3,0.5,3.25,2.5,5.25\n";
                
                REQUIRE(expected.view() == observation_log.view());
            }
        }
    }
}
