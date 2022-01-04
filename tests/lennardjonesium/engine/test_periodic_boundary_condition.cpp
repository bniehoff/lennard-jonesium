/**
 * Test PeriodicBoundaryCondition
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <lennardjonesium/tools/bounding_box.hpp>
#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/engine/periodic_boundary_condition.hpp>

SCENARIO("Imposing boundary conditions")
{
    // System of 3 particles
    physics::SystemState state(3);

    // Set the positions
    state.positions = Eigen::Matrix4Xd{
        {1.7,   0,    0},
        {0,   3.2,    0},
        {0,     0, -4.3},
        {0,     0,    0}
    };

    // Set up a box of side length 1.5
    tools::BoundingBox bounding_box{1.5};

    // Set up a PeriodicBoundaryCondition on this BoundingBox
    engine::PeriodicBoundaryCondition pbc{bounding_box};

    WHEN("I apply the boundary conditions to the state")
    {
        state | pbc;

        THEN("The particle positions are set to the correct locations within the box")
        {
            REQUIRE(Approx(0.2) == state.positions(0, 0));
            REQUIRE(Approx(0.2) == state.positions(1, 1));
            REQUIRE(Approx(0.2) == state.positions(2, 2));
        }
    }
}
