/**
 * Test functionality of different Sinks.
 */

#include <catch2/catch.hpp>

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
