/**
 * Test ObservationPhase
 */

#include <ranges>
#include <string>
#include <variant>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/system_parameters.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/transformations.hpp>
#include <src/lennardjonesium/physics/measurements.hpp>
#include <src/lennardjonesium/engine/simulation_phase.hpp>

SCENARIO("Observation Phase decision-making")
{
    // Define system parameters
    tools::SystemParameters system_parameters{
        .temperature {0.5},
        .density {1.0},
        .particle_count {50}
    };

    // Create a state with some velocities
    physics::SystemState state(system_parameters.particle_count);

    state.positions.setRandom();
    state.velocities.setRandom();

    // Create measurement object
    physics::ThermodynamicMeasurement measurement;

    // Create observation parameters
    engine::ObservationParameters observation_parameters{
        .sample_size {2},
        .observation_interval {10},
        .observation_count {10}
    };

    int start_time{0};

    // Create the ObservationPhase object
    engine::ObservationPhase observation_phase{
        start_time,
        system_parameters,
        observation_parameters
    };

    WHEN("I pass a time step that is before the first observation interval")
    {
        state | measurement;

        auto commands = observation_phase.evaluate(
            observation_parameters.observation_interval - 3,
            measurement
        );

        THEN("I get no response")
        {
            REQUIRE(commands.empty());
        }
    }

    WHEN("I reach an observation time and the temperature is within the desired range")
    {
        state | physics::set_temperature(system_parameters.temperature) | measurement;

        auto commands = observation_phase.evaluate(
            observation_parameters.observation_interval - 1,
            measurement
        );

        THEN("The command at observation_interval - 1 should be empty")
        {
            REQUIRE(commands.empty());
        }

        commands = observation_phase.evaluate(
            observation_parameters.observation_interval,
            measurement
        );

        THEN("The command at observation_interval should be to record an observation")
        {
            REQUIRE(1 == commands.size());
            REQUIRE(std::holds_alternative<engine::RecordObservation>(commands.front()));
        }
    }

    WHEN("I make the desired number of observations")
    {
        std::vector<engine::Command> commands;

        state | physics::set_temperature(system_parameters.temperature) | measurement;

        // We need to run the "simulation" from the beginning, with fixed temperature
        int total_time = (
            observation_parameters.observation_interval * observation_parameters.observation_count
        );

        for (int time_step : std::views::iota(0, total_time))
        {
            commands = observation_phase.evaluate(time_step, measurement);
            
            if ((time_step > 0)
                    && (time_step % observation_parameters.observation_interval == 0))
            {
                REQUIRE(1 == commands.size());
                REQUIRE(std::holds_alternative<engine::RecordObservation>(commands.front()));
            }
            else
            {
                REQUIRE(commands.empty());
            }
        }

        // Now execute the final step
        commands = observation_phase.evaluate(total_time, measurement);

        THEN("The final list of commands should indicate success")
        {
            REQUIRE(2 == commands.size());
            REQUIRE(std::holds_alternative<engine::RecordObservation>(commands[0]));
            REQUIRE(std::holds_alternative<engine::PhaseComplete>(commands[1]));
        }
    }

    WHEN("I measure an average temperature that is outside the desired range")
    {
        state | physics::set_temperature(system_parameters.temperature * 2) | measurement;

        auto commands = observation_phase.evaluate(
            observation_parameters.observation_interval - 1,
            measurement
        );

        THEN("The command at observation_interval - 1 should be empty")
        {
            REQUIRE(commands.empty());
        }

        commands = observation_phase.evaluate(
            observation_parameters.observation_interval,
            measurement
        );

        THEN("The command at observation_interval should be to abort")
        {
            REQUIRE(1 == commands.size());
            REQUIRE(std::holds_alternative<engine::AbortSimulation>(commands.front()));
        }
    }
}
