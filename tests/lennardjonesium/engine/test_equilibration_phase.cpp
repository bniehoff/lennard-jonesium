/**
 * Test EquilibrationPhase
 */

#include <ranges>
#include <string>
#include <variant>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/transformations.hpp>
#include <src/lennardjonesium/physics/measurements.hpp>
#include <src/lennardjonesium/engine/simulation_phase.hpp>

SCENARIO("Equilibration Phase decision-making")
{
    // Create a state with some velocities
    physics::SystemState state(50);

    state.positions.setRandom();
    state.velocities.setRandom();

    // Create thermodynamics object
    physics::Thermodynamics thermodynamics;

    // Create the equilibration parameters
    engine::EquilibrationPhase::Parameters parameters{};

    parameters.sample_size = 2;
    parameters.assessment_interval = 10;
    parameters.steady_state_time = 100;
    parameters.timeout = 500;

    int start_time{0};
    double target_temperature{0.5};

    // Create the EquilibrationPhase object
    engine::EquilibrationPhase equilibration_phase{
        start_time,
        target_temperature,
        parameters
    };

    WHEN("I pass a time step that is before the first adjustment interval")
    {
        engine::Command command;
        state | thermodynamics;

        command = equilibration_phase.evaluate(parameters.assessment_interval - 3, thermodynamics);

        THEN("I get no response")
        {
            REQUIRE(std::holds_alternative<std::monostate>(command));
        }
    }

    WHEN("I measure an average temperature that is outside the desired range")
    {
        engine::Command command;
        state | physics::set_temperature(target_temperature * 2) | thermodynamics;

        command = equilibration_phase.evaluate(parameters.assessment_interval - 1, thermodynamics);

        THEN("The command at assessment_interval - 1 should be empty")
        {
            REQUIRE(std::holds_alternative<std::monostate>(command));
        }

        command = equilibration_phase.evaluate(parameters.assessment_interval, thermodynamics);

        THEN("The command at assessment_interval should be to set the temperature")
        {
            REQUIRE(std::holds_alternative<engine::SetTemperature>(command));
            REQUIRE(
                Approx(target_temperature) == std::get<engine::SetTemperature>(command).temperature
            );
        }
    }

    WHEN("I measure the correct temperature at the adjustment interval")
    {
        engine::Command command;
        state | physics::set_temperature(target_temperature) | thermodynamics;

        command = equilibration_phase.evaluate(parameters.assessment_interval - 1, thermodynamics);

        THEN("The command at assessment_interval - 1 should be empty")
        {
            REQUIRE(std::holds_alternative<std::monostate>(command));
        }

        command = equilibration_phase.evaluate(parameters.assessment_interval, thermodynamics);

        THEN("The command at assessment_interval should also be empty")
        {
            REQUIRE(std::holds_alternative<std::monostate>(command));
        }
    }

    WHEN("I measure the correct temperature at the steady state time")
    {
        engine::Command command;
        state | physics::set_temperature(target_temperature) | thermodynamics;

        // We need to run the "simulation" from the beginning, with fixed temperature
        for (int time_step : std::views::iota(0, parameters.steady_state_time))
        {
            command = equilibration_phase.evaluate(time_step, thermodynamics);
            REQUIRE(std::holds_alternative<std::monostate>(command));
        }

        // Now execute the final step
        command = equilibration_phase.evaluate(parameters.steady_state_time, thermodynamics);

        THEN("The command at steady_state_time should indicate success")
        {
            REQUIRE(std::holds_alternative<engine::PhaseComplete>(command));
        }
    }

    WHEN("I measure the wrong temperature at timeout")
    {
        engine::Command command;

        state | physics::set_temperature(target_temperature * 2) | thermodynamics;

        // We need to force adjustments to happen over the entire time evolution up until timeout
        for (int time_step : std::views::iota(0, parameters.timeout))
        {
            command = equilibration_phase.evaluate(time_step, thermodynamics);

            if ((time_step > 0) && (time_step % parameters.assessment_interval == 0)) {
                REQUIRE(std::holds_alternative<engine::SetTemperature>(command));
            } else {
                REQUIRE(std::holds_alternative<std::monostate>(command));
            }
        }

        // Now execute the final step

        command = equilibration_phase.evaluate(parameters.timeout, thermodynamics);

        THEN("The command at timeout should indicate failure")
        {
            REQUIRE(std::holds_alternative<engine::AbortSimulation>(command));
        }
    }
}
