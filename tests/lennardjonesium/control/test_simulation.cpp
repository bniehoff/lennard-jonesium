/**
 * Test Simulation.  This is more of an integration test.  We should have enough of the pieces
 * put together to run a complete simulation.
 */

#include <ranges>
#include <utility>
#include <memory>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/math.hpp>
#include <src/lennardjonesium/tools/moving_sample.hpp>
#include <src/lennardjonesium/tools/bounding_box.hpp>
#include <src/lennardjonesium/tools/system_parameters.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/measurements.hpp>
#include <src/lennardjonesium/engine/particle_pair_filter.hpp>
#include <src/lennardjonesium/engine/force_calculation.hpp>
#include <src/lennardjonesium/engine/boundary_condition.hpp>
#include <src/lennardjonesium/engine/initial_condition.hpp>
#include <src/lennardjonesium/engine/integrator.hpp>
#include <src/lennardjonesium/control/simulation_phase.hpp>
#include <src/lennardjonesium/control/simulation.hpp>

#include <tests/mock/constant_short_range_force.hpp>

double measure_temperature(physics::SystemState state, const engine::Integrator& integrator)
{
    /**
     * To understand whether equilibration was successful, we cannot just measure the instantanous
     * temperature of the final state, because this is a single microstate and might not be
     * representative of the thermodynamic temperature.  Instead we have to evolve the system
     * for some time, take temperature samples, and compute the average temperature.
     */

    int samples = 20;
    int sample_interval = 5;

    tools::MovingSample<double> temperature_samples(samples);

    for ([[maybe_unused]] int i : std::views::iota(0, samples))
    {
        state | integrator(sample_interval);
        temperature_samples.push_back(physics::temperature(state));
    }

    return temperature_samples.statistics().mean;
}


SCENARIO("Equilibrating the system")
{
    /**
     * First create the initial condition.  We will deliberately create an initial condition with
     * the wrong temperature, so that we can test that the equilibrator rescales appropriately.
     */

    // Define system parameters
    tools::SystemParameters system_parameters{
        .temperature {0.2},
        .density {0.8},
        .particle_count {500}
    };

    // Target system parameters are different (this is not normal)
    auto target_system_parameters = system_parameters;
    target_system_parameters.temperature = 0.8;

    // This always uses the default random seed, so the test is repeatable
    engine::InitialCondition initial_condition(system_parameters);

    // Now set up the Integrator (could really use a factory method for this)
    double force = 4.0;
    double cutoff_distance = 2.0;
    double time_step = 0.005;

    tools::BoundingBox bounding_box = initial_condition.bounding_box();

    mock::ConstantShortRangeForce short_range_force{force, cutoff_distance};
    engine::CellListParticlePairFilter particle_pair_filter(bounding_box, cutoff_distance);

    engine::ShortRangeForceCalculation force_calculation{short_range_force, particle_pair_filter};
    engine::PeriodicBoundaryCondition boundary_condition(bounding_box);

    engine::VelocityVerletIntegrator integrator(time_step, boundary_condition, force_calculation);

    GIVEN("An EquilibrationPhase with a large tolerance")
    {
        control::EquilibrationPhase::Parameters equilibration_parameters{
            .tolerance {0.10},
            .sample_size {50},
            .adjustment_interval {100},
            .steady_state_time {5000},
            .timeout {50000}
        };

        control::Simulation::Schedule schedule;

        schedule.push(
            std::make_unique<control::EquilibrationPhase>(
                "Large-tolerance Equilibration Phase",
                target_system_parameters,
                equilibration_parameters
            )
        );

        control::Simulation simulation(integrator, std::move(schedule));

        WHEN("I run the simulation")
        {
            auto state = initial_condition.system_state();

            state | simulation;

            THEN("The system equilibrates to the desired temperature")
            {
                REQUIRE(
                    tools::relative_error(
                        measure_temperature(state, integrator),
                        target_system_parameters.temperature
                    ) < equilibration_parameters.tolerance
                );
            }
        }
    }
}
