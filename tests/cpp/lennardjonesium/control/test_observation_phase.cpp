/**
 * Test ObservationPhase
 */

#include <ranges>
#include <string>
#include <variant>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/cpp/lennardjonesium/tools/system_parameters.hpp>
#include <src/cpp/lennardjonesium/physics/system_state.hpp>
#include <src/cpp/lennardjonesium/physics/transformations.hpp>
#include <src/cpp/lennardjonesium/physics/measurements.hpp>
#include <src/cpp/lennardjonesium/physics/observation.hpp>
#include <src/cpp/lennardjonesium/control/command_queue.hpp>
#include <src/cpp/lennardjonesium/control/simulation_phase.hpp>

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
    control::ObservationPhase::Parameters observation_parameters{
        .sample_size {2},
        .observation_interval {10},
        .observation_count {10}
    };

    int start_time{37};

    // Create the ObservationPhase object
    control::ObservationPhase observation_phase{
        "Test Observation Phase",
        system_parameters,
        observation_parameters
    };

    observation_phase.set_start_time(start_time);

    // Create empty command queue
    control::CommandQueue command_queue;

    WHEN("I pass a time step that is before the first observation interval")
    {
        state | measurement;

        observation_phase.evaluate(
            command_queue,
            start_time + observation_parameters.observation_interval - 3,
            measurement
        );

        THEN("I get the default time advance command")
        {
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
        }
    }

    WHEN("I reach an observation time and the temperature is within the desired range")
    {
        state | physics::set_temperature(system_parameters.temperature) | measurement;

        observation_phase.evaluate(
            command_queue,
            start_time + observation_parameters.observation_interval - 1,
            measurement
        );

        THEN("The command at observation_interval - 1 should be AdvanceTime")
        {
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
        }

        command_queue.pop();
        observation_phase.evaluate(
            command_queue,
            start_time + observation_parameters.observation_interval,
            measurement
        );

        THEN("The command at observation_interval should be to record an observation")
        {
            REQUIRE(2 == command_queue.size());
            REQUIRE(std::holds_alternative<control::RecordObservation>(command_queue.front()));
            command_queue.pop();
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
        }
    }

    WHEN("I make the desired number of observations")
    {
        state | physics::set_temperature(system_parameters.temperature) | measurement;

        // We need to run the "simulation" from the beginning, with fixed temperature
        int total_time = (
            observation_parameters.observation_interval * observation_parameters.observation_count
        );

        for (int time_step : std::views::iota(0, total_time))
        {
            observation_phase.evaluate(command_queue, start_time + time_step, measurement);
            
            if ((time_step > 0)
                    && (time_step % observation_parameters.observation_interval == 0))
            {
                REQUIRE(2 == command_queue.size());
                REQUIRE(std::holds_alternative<control::RecordObservation>(command_queue.front()));
                command_queue.pop();
                REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
            }
            else
            {
                REQUIRE(1 == command_queue.size());
                REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
            }

            command_queue.pop();
        }

        // Now execute the final step
        observation_phase.evaluate(command_queue, start_time + total_time, measurement);

        THEN("The final list of commands should indicate success")
        {
            REQUIRE(2 == command_queue.size());
            REQUIRE(std::holds_alternative<control::RecordObservation>(command_queue.front()));
            command_queue.pop();
            REQUIRE(std::holds_alternative<control::PhaseComplete>(command_queue.front()));
        }
    }

    WHEN("I measure an average temperature that is outside the desired range")
    {
        state | physics::set_temperature(system_parameters.temperature * 2) | measurement;

        observation_phase.evaluate(
            command_queue,
            start_time + observation_parameters.observation_interval - 1,
            measurement
        );

        THEN("The command at adjustment_interval - 1 should be AdvanceTime")
        {
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
        }

        command_queue.pop();
        observation_phase.evaluate(
            command_queue,
            start_time + observation_parameters.observation_interval,
            measurement
        );

        THEN("The command at observation_interval should be to abort")
        {
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AbortSimulation>(command_queue.front()));
        }
    }
}
