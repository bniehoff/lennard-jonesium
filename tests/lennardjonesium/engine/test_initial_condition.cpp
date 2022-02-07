/**
 * Test the InitialCondition class
 */

#include <random>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/bounding_box.hpp>
#include <src/lennardjonesium/tools/system_parameters.hpp>
#include <src/lennardjonesium/tools/cubic_lattice.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/derived_properties.hpp>
#include <src/lennardjonesium/engine/initial_condition.hpp>

SCENARIO("Creating initial conditions with specified properties")
{
    tools::SystemParameters system_parameters{
        .temperature{0.5},
        .density{0.8},
        .particle_count{50}
    };

    std::random_device rd{};

    WHEN("I create an initial condition")
    {
        engine::InitialCondition initial_condition{
            system_parameters, tools::CubicLattice::BodyCentered(), rd()
        };

        THEN("The resulting initial_condition has the correct properties")
        {
            tools::BoundingBox bounding_box = initial_condition.bounding_box();
            physics::SystemState system_state = initial_condition.system_state();

            REQUIRE(Approx(system_parameters.density) ==
                static_cast<double>(system_parameters.particle_count) / bounding_box.volume()
            );

            REQUIRE(Approx(system_parameters.temperature) == physics::temperature(system_state));

            // NOTE: Approx(0) cannot be used, approximation is proportional to the value!
            
            REQUIRE(Approx(1.0) == 1.0 + physics::total_momentum(system_state).squaredNorm());

            REQUIRE(Approx(1.0) == 1.0 + 
                physics::total_angular_momentum(system_state).squaredNorm()
            );
        }

        THEN("The fourth velocity component is zero")
        {
            REQUIRE(initial_condition.system_state().velocities.bottomRows<1>().isZero());
        }
    }
}
