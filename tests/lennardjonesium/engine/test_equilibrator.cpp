/**
 * Test Equilibrator.  This is really more of an integration test.  We have to put almost all
 * the pieces of the system together.
 */

#include <ranges>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/math.hpp>
#include <src/lennardjonesium/tools/moving_average.hpp>
#include <src/lennardjonesium/tools/bounding_box.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/measurements.hpp>
#include <src/lennardjonesium/engine/particle_pair_filter.hpp>
#include <src/lennardjonesium/engine/force_calculation.hpp>
#include <src/lennardjonesium/engine/boundary_condition.hpp>
#include <src/lennardjonesium/engine/initial_condition.hpp>
#include <src/lennardjonesium/engine/integrator.hpp>
#include <src/lennardjonesium/engine/equilibrator.hpp>

#include <tests/lennardjonesium/test_utils/constant_short_range_force.hpp>

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

    tools::MovingAverage<double> temperature_samples(samples);

    for ([[maybe_unused]] int i : std::views::iota(0, samples))
    {
        state | integrator(sample_interval);
        temperature_samples.push_back(physics::temperature(state));
    }

    return temperature_samples.average();
}

SCENARIO("Equilibrating the system")
{
    /**
     * First create the initial condition.  We will deliberately create an initial condition with
     * the wrong temperature, so that we can test that the equilibrator rescales appropriately.
     */
    int particle_count = 500;
    double density = 0.8;
    double initial_temperature = 0.2;
    double target_temperature = 0.8;

    // This always uses the default random seed, so the test is repeatable
    engine::InitialCondition initial_condition(particle_count, density, initial_temperature);

    // Now set up the Integrator
    double force = -4.0;
    double cutoff_distance = 2.0;
    double time_step = 0.005;

    tools::BoundingBox bounding_box = initial_condition.bounding_box();

    ConstantShortRangeForce short_range_force{force, cutoff_distance};
    engine::CellListParticlePairFilter particle_pair_filter(bounding_box, cutoff_distance);

    engine::ShortRangeForceCalculation force_calculation{short_range_force, particle_pair_filter};
    engine::PeriodicBoundaryCondition boundary_condition(bounding_box);

    engine::VelocityVerletIntegrator integrator(time_step, boundary_condition, force_calculation);

    WHEN("I attempt to equilibrate with a large tolerance")
    {
        physics::SystemState state = initial_condition.system_state();
        engine::Equilibrator::Parameters parameters{};

        parameters.tolerance = 0.10;

        engine::Equilibrator equilibrator{integrator, parameters};

        THEN("The system equilibrates")
        {
            try
            {
                state | equilibrator(target_temperature);
            }
            catch(const engine::EquilibrationError& e)
            {
                FAIL("Failed to equilibrate");
            }

            REQUIRE(
                tools::relative_error(measure_temperature(state, integrator), target_temperature)
                < parameters.tolerance
            );
        }
    }

    WHEN("I attempt to equilibrate with a small tolerance")
    {
        physics::SystemState state = initial_condition.system_state();
        engine::Equilibrator::Parameters parameters{};

        parameters.tolerance = 0.0000001;

        engine::Equilibrator equilibrator{integrator, parameters};

        THEN("The system fails to equilibrate")
        {
            REQUIRE_THROWS_AS(
                state | equilibrator(target_temperature),
                engine::EquilibrationError
            );
        }
    }
}
