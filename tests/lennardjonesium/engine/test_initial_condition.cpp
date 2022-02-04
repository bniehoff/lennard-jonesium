/**
 * Test the InitialCondition class
 */

#include <random>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/bounding_box.hpp>
#include <src/lennardjonesium/tools/cubic_lattice.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/derived_properties.hpp>
#include <src/lennardjonesium/engine/initial_condition.hpp>

SCENARIO("Creating initial conditions with specified properties")
{
    int particle_count{50};
    double density{0.8};
    double temperature{0.5};

    std::random_device rd{};

    WHEN("I create an initial condition")
    {
        engine::InitialCondition initial_condition{
            particle_count, density, temperature, tools::CubicLattice::BodyCentered(), rd()
        };

        THEN("The resulting initial_condition has the correct properties")
        {
            tools::BoundingBox bounding_box = initial_condition.bounding_box();
            physics::SystemState system_state = initial_condition.system_state();

            REQUIRE(Approx(density) ==
                static_cast<double>(particle_count) / bounding_box.volume()
            );

            REQUIRE(Approx(temperature) == physics::temperature(system_state));

            // NOTE: Approx(0) cannot be used, approximation is proportional to the value!
            
            REQUIRE(Approx(1.0) == 1.0 + physics::total_momentum(system_state).squaredNorm());

            REQUIRE(Approx(1.0) == 1.0 + 
                physics::total_angular_momentum(system_state).squaredNorm()
            );
        }
    }
}
