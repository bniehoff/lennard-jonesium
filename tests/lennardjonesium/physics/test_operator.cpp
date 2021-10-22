/**
 * Test Operator concept
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/physics/operator.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>

using Eigen::Matrix4Xd;
using Eigen::Vector4d;

using physics::SystemState;
using physics::Operator;

SystemState& increase_velocity(SystemState& s)
{
    s.velocities.colwise() += Vector4d::UnitZ();
    return s;
}

SCENARIO( "Piping Operators" ) {
    SystemState s(1);
    Vector4d gravity{0, 0, -10, 0};

    auto apply_gravity = [gravity](SystemState& s) -> SystemState& {
        s.velocities.colwise() += gravity;
        return s;
    };

    WHEN( "I apply a chain of functions to the state" ) {
        s | increase_velocity | increase_velocity | apply_gravity;

        THEN( "I get the expected result" ) {
            REQUIRE( Vector4d{0, 0, -8, 0} == s.velocities.col(0) );
        }
    }

    WHEN( "I act with the identity operator" ) {
        s | physics::identity_operator;

        THEN( "I get back the state I started with" ) {
            REQUIRE( Vector4d{0, 0, 0, 0} == s.velocities.col(0) );
        }
    }
}
