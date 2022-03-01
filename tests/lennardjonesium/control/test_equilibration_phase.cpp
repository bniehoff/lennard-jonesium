/**
 * Test EquilibrationPhase
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
#include <src/lennardjonesium/control/simulation_phase.hpp>

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
    control::EquilibrationParameters equilibration_parameters{
        .sample_size {2},
        .adjustment_interval {10},
        .steady_state_time {100},
        .timeout {500}
    };

    int start_time{0};

    // Create the EquilibrationPhase object
    control::EquilibrationPhase equilibration_phase{
        start_time,
        system_parameters,
        equilibration_parameters
    };

    WHEN("I pass a time step that is before the first adjustment interval")
    {
        state | measurement;

        auto commands = equilibration_phase.evaluate(
            equilibration_parameters.adjustment_interval - 3,
            measurement
        );

        THEN("I get no response")
        {
            REQUIRE(commands.empty());
        }
    }

    WHEN("I measure an average temperature that is outside the desired range")
    {
        state | physics::set_temperature(system_parameters.temperature * 2) | measurement;

        auto commands = equilibration_phase.evaluate(
            equilibration_parameters.adjustment_interval - 1,
            measurement
        );

        THEN("The command at adjustment_interval - 1 should be empty")
        {
            REQUIRE(commands.empty());
        }

        commands = equilibration_phase.evaluate(
            equilibration_parameters.adjustment_interval,
            measurement
        );

        THEN("The command at adjustment_interval should be to set the temperature")
        {
            REQUIRE(1 == commands.size());
            REQUIRE(std::holds_alternative<control::AdjustTemperature>(commands.front()));
            REQUIRE(
                Approx(system_parameters.temperature)
                    == std::get<control::AdjustTemperature>(commands.front()).temperature
            );
        }
    }

    WHEN("I measure the correct temperature at the adjustment interval")
    {
        state | physics::set_temperature(system_parameters.temperature) | measurement;

        auto commands = equilibration_phase.evaluate(
            equilibration_parameters.adjustment_interval - 1,
            measurement
        );

        THEN("The command at adjustment_interval - 1 should be empty")
        {
            REQUIRE(commands.empty());
        }

        commands = equilibration_phase.evaluate(
            equilibration_parameters.adjustment_interval,
            measurement
        );

        THEN("The command at adjustment_interval should also be empty")
        {
            REQUIRE(commands.empty());
        }
    }

    WHEN("I measure the correct temperature at the steady state time")
    {
        std::vector<control::Command> commands;

        state | physics::set_temperature(system_parameters.temperature) | measurement;

        // We need to run the "simulation" from the beginning, with fixed temperature
        for (int time_step : std::views::iota(0, equilibration_parameters.steady_state_time))
        {
            commands = equilibration_phase.evaluate(time_step, measurement);
            REQUIRE(commands.empty());
        }

        // Now execute the final step
        commands = equilibration_phase.evaluate(
            equilibration_parameters.steady_state_time,
            measurement
        );

        THEN("The command at steady_state_time should indicate success")
        {
            REQUIRE(1 == commands.size());
            REQUIRE(std::holds_alternative<control::PhaseComplete>(commands.front()));
        }
    }

    WHEN("I measure the wrong temperature at timeout")
    {
        std::vector<control::Command> commands;

        state | physics::set_temperature(system_parameters.temperature * 2) | measurement;

        // We need to force adjustments to happen over the entire time evolution up until timeout
        for (int time_step : std::views::iota(0, equilibration_parameters.timeout))
        {
            commands = equilibration_phase.evaluate(time_step, measurement);

            if ((time_step > 0)
                    && (time_step % equilibration_parameters.adjustment_interval == 0))
            {
                REQUIRE(1 == commands.size());
                REQUIRE(std::holds_alternative<control::AdjustTemperature>(commands.front()));
            }
            else
            {
                REQUIRE(commands.empty());
            }
        }

        // Now execute the final step

        commands = equilibration_phase.evaluate(equilibration_parameters.timeout, measurement);

        THEN("The command at timeout should indicate failure")
        {
            // Note that a temperature adjustment command will also be issued, since the
            // temperature is outside the desired range
            REQUIRE(2 == commands.size());
            REQUIRE(std::holds_alternative<control::AdjustTemperature>(commands[0]));
            REQUIRE(std::holds_alternative<control::AbortSimulation>(commands[1]));
        }
    }
}
