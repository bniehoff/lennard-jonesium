/**
 * Test for PeriodicBoundaryCondition
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/engine/periodic_boundary_condition.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/operator.hpp>

using Eigen::Matrix4Xd;
using Eigen::Vector4d;
using engine::PeriodicBoundaryCondition;
using physics::SystemState;

SCENARIO( "Imposing boundary conditions" ) {
    // System of 3 particles
    SystemState state(3);

    // Set the positions
    state.positions = Matrix4Xd{
        {1.7,   0,    0},
        {0,   3.2,    0},
        {0,     0, -4.3},
        {0,     0,    0}
    };

    // Use a bounding box of side length 1.5
    PeriodicBoundaryCondition boundary_condition(1.5);

    WHEN( "I impose the boundary conditions on the system" ) {
        state | boundary_condition;

        THEN( "The particle positions are set to the correct locations within the box" ) {
            REQUIRE( Approx(0.2) == state.positions(0, 0) );
            REQUIRE( Approx(0.2) == state.positions(1, 1) );
            REQUIRE( Approx(0.2) == state.positions(2, 2) );
        }
    }
}
