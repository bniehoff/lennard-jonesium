/**
 * A fairly trivial test, since SystemState is just data.
 */

#include <ranges>

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

    WHEN("I set the number of particles at runtime")
    {
        SystemState s;

        s.set_particle_count(7);

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

        for (const auto i : std::views::iota(0, count)) {
            s.positions.col(i) = Vector4d{i + 0.0, i + 1.0, i + 2.0, i + 3.0};
        }

        THEN("I can read back the same vectors")
        {
            REQUIRE(Vector4d{3, 4, 5, 6} == s.positions.col(3));
        }
    }

    GIVEN("A state with some dynamical quantities")
    {
        int count = 3;
        SystemState s(count);

        s.forces = Eigen::Matrix4Xd{
            {3, 4, 5}, {2, 4, 6}, {3, 6, 9}, {0, 0, 0}
        };
        s.potential_energy = 107.2;
        s.virial = 123.3;

        WHEN("I reset the dynamical quantities")
        {
            s.reset_dynamical_quantities();

            THEN("They are all zero")
            {
                Eigen::Matrix4Xd expected_forces{{0, 0, 0}, {0, 0, 0,}, {0, 0, 0}, {0, 0, 0}};

                REQUIRE(expected_forces == s.forces);
                REQUIRE(0 == s.potential_energy);
                REQUIRE(0 == s.virial);
            }
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
