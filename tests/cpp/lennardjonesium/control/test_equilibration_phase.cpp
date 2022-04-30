/**
 * Test EquilibrationPhase
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
#include <src/cpp/lennardjonesium/control/command_queue.hpp>
#include <src/cpp/lennardjonesium/control/simulation_phase.hpp>

SCENARIO("Equilibration Phase decision-making")
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

    // Create the equilibration parameters
    control::EquilibrationPhase::Parameters equilibration_parameters{
        .sample_size {2},
        .adjustment_interval {10},
        .steady_state_time {100},
        .timeout {500}
    };

    int start_time{1337};

    // Create the EquilibrationPhase object
    control::EquilibrationPhase equilibration_phase{
        "Test Equilibration Phase",
        system_parameters,
        equilibration_parameters
    };

    equilibration_phase.set_start_time(start_time);

    // Create empty command queue
    control::CommandQueue command_queue;

    WHEN("I pass a time step that is before the first adjustment interval")
    {
        state | measurement;

        equilibration_phase.evaluate(
            command_queue,
            start_time + equilibration_parameters.adjustment_interval - 3,
            measurement
        );

        THEN("I get the default time advance command")
        {
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
        }
    }

    WHEN("I measure an average temperature that is outside the desired range")
    {
        state | physics::set_temperature(system_parameters.temperature * 2) | measurement;

        equilibration_phase.evaluate(
            command_queue,
            start_time + equilibration_parameters.adjustment_interval - 1,
            measurement
        );

        THEN("The command at adjustment_interval - 1 should be AdvanceTime")
        {
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
        }

        command_queue.pop();
        equilibration_phase.evaluate(
            command_queue,
            start_time + equilibration_parameters.adjustment_interval,
            measurement
        );

        THEN("The command at adjustment_interval should be to set the temperature")
        {
            REQUIRE(2 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdjustTemperature>(command_queue.front()));
            REQUIRE(
                Approx(system_parameters.temperature)
                    == std::get<control::AdjustTemperature>(
                        command_queue.front()
                    ).target_temperature
            );
            command_queue.pop();
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
        }
    }

    WHEN("I measure the correct temperature at the adjustment interval")
    {
        state | physics::set_temperature(system_parameters.temperature) | measurement;

        equilibration_phase.evaluate(
            command_queue,
            start_time + equilibration_parameters.adjustment_interval - 1,
            measurement
        );

        THEN("The command at adjustment_interval - 1 should be AdvanceTime")
        {
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
        }

        command_queue.pop();
        equilibration_phase.evaluate(
            command_queue,
            start_time + equilibration_parameters.adjustment_interval,
            measurement
        );

        THEN("The command at adjustment_interval should also be AdvanceTime")
        {
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
        }
    }

    WHEN("I measure the correct temperature at the steady state time")
    {
        state | physics::set_temperature(system_parameters.temperature) | measurement;

        // We need to run the "simulation" from the beginning, with fixed temperature
        for (int time_step : std::views::iota(0, equilibration_parameters.steady_state_time))
        {
            equilibration_phase.evaluate(command_queue, start_time + time_step, measurement);
            
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdvanceTime>(command_queue.front()));
            
            command_queue.pop();
        }

        // Now execute the final step
        equilibration_phase.evaluate(
            command_queue,
            start_time + equilibration_parameters.steady_state_time,
            measurement
        );

        THEN("The command at steady_state_time should indicate success")
        {
            REQUIRE(1 == command_queue.size());
            REQUIRE(std::holds_alternative<control::PhaseComplete>(command_queue.front()));
        }
    }

    WHEN("I measure the wrong temperature at timeout")
    {
        state | physics::set_temperature(system_parameters.temperature * 2) | measurement;

        // We need to force adjustments to happen over the entire time evolution up until timeout
        for (int time_step : std::views::iota(0, equilibration_parameters.timeout))
        {
            equilibration_phase.evaluate(command_queue, start_time + time_step, measurement);

            if ((time_step > 0)
                    && (time_step % equilibration_parameters.adjustment_interval == 0))
            {
                REQUIRE(2 == command_queue.size());
                REQUIRE(std::holds_alternative<control::AdjustTemperature>(command_queue.front()));
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

        equilibration_phase.evaluate(
            command_queue, start_time + equilibration_parameters.timeout, measurement
        );

        THEN("The command at timeout should indicate failure")
        {
            // Note that a temperature adjustment command will also be issued, since the
            // temperature is outside the desired range
            REQUIRE(2 == command_queue.size());
            REQUIRE(std::holds_alternative<control::AdjustTemperature>(command_queue.front()));
            command_queue.pop();
            REQUIRE(std::holds_alternative<control::AbortSimulation>(command_queue.front()));
        }
    }
}
