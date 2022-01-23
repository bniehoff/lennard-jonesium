/**
 * A fairly trivial test, since SystemState is just data.
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/physics/system_state.hpp>

using Eigen::Matrix4Xd;
using Eigen::Vector4d;

using physics::SystemState;
using physics::Operator;

SCENARIO("Representing the system state")
{
    WHEN("I create a SystemState for a given number of particles")
    {
        SystemState s(7);

        THEN("It has the expected number of particles")
        {
            REQUIRE(7 == s.positions.cols());
            REQUIRE(7 == s.velocities.cols());
            REQUIRE(7 == s.forces.cols());
            REQUIRE(7 == s.displacements.cols());
        }
    }

    WHEN("I set some of the column vectors")
    {
        int count = 7;
        SystemState s(count);

        for (int i = 0; i < count; ++i)
        {
            s.positions.col(i) = Vector4d{i + 0.0, i + 1.0, i + 2.0, i + 3.0};
        }

        THEN("I can read back the same vectors")
        {
            REQUIRE(Vector4d{3, 4, 5, 6} == s.positions.col(3));
        }
    }

    WHEN("I copy a system state")
    {
        SystemState s1{1};

        s1.positions.col(0) = Vector4d{0, 0, 1, 0};

        SystemState s2 = s1;

        THEN("The two positions matrices are equal")
        {
            REQUIRE(s1.positions.isApprox(s2.positions));
        }
    }

    WHEN("I copy a system state and change a value")
    {
        SystemState s1{1};

        s1.positions.col(0) = Vector4d{0, 0, 1, 0};

        SystemState s2 = s1;

        s2.positions.col(0) = Vector4d{0, 0, 2, 0};

        THEN("The two positions matrices are not equal")
        {
            REQUIRE_FALSE(s1.positions.isApprox(s2.positions));
        }
    }
}

SystemState& increase_velocity(SystemState& s)
{
    s.velocities.colwise() += Vector4d::UnitZ();
    return s;
}

SCENARIO("Piping Operators")
{
    SystemState s(1);
    Vector4d gravity{0, 0, -10, 0};

    auto apply_gravity = [gravity](SystemState& s) -> SystemState& {
        s.velocities.colwise() += gravity;
        return s;
    };

    WHEN("I apply a chain of functions to the state")
    {
        s | increase_velocity | increase_velocity | apply_gravity;

        THEN("I get the expected result")
        {
            REQUIRE(Vector4d{0, 0, -8, 0} == s.velocities.col(0));
        }
    }

    WHEN("I act with the identity operator")
    {
        s | physics::identity_operator;

        THEN("I get back the state I started with")
        {
            REQUIRE(Vector4d{0, 0, 0, 0} == s.velocities.col(0));
        }
    }
}
