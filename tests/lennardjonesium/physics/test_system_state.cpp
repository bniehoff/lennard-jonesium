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
using physics::Measurement;
using physics::Property;

template<typename First, typename Second>
constexpr bool pipe_compiles = requires (SystemState s, First p1, Second p2)
{
    s | p1 | p2;
};

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

SCENARIO("Piping Operators and Measurements")
{
    SystemState s(1);
    Vector4d gravity{0, 0, -10, 0};
    Vector4d velocity{0, 0, 0, 0};
    double speed{};

    auto apply_gravity = [gravity](SystemState& s) -> SystemState& {
        s.velocities.colwise() += gravity;
        return s;
    };

    auto measure_speed = [&speed] (const SystemState& s) -> const SystemState& {
        speed = s.velocities.col(0).norm();
        return s;
    };

    auto measure_velocity = [&velocity] (const SystemState& s) -> const SystemState& {
        velocity = s.velocities.col(0);
        return s;
    };

    WHEN("I apply a chain of functions to the state")
    {
        auto particle_count = s | increase_velocity | increase_velocity | apply_gravity
                                | measure_velocity | measure_speed | physics::particle_count;

        THEN("I get the expected result")
        {
            REQUIRE(Vector4d{0, 0, -8, 0} == velocity);
            REQUIRE(Approx(8) == speed);
            REQUIRE(1 == particle_count);
        }
    }

    WHEN("I act with the identity operator")
    {
        s | physics::identity_operator | measure_velocity;

        THEN("I get back the state I started with")
        {
            REQUIRE(Vector4d{0, 0, 0, 0} == velocity);
        }
    }

    WHEN("I try different combinations of piping orders")
    {
        REQUIRE(pipe_compiles<decltype(apply_gravity), decltype(measure_velocity)>);
        REQUIRE(pipe_compiles<decltype(apply_gravity), decltype(physics::particle_count)>);
        REQUIRE(pipe_compiles<decltype(measure_velocity), decltype(physics::particle_count)>);
        
        REQUIRE_FALSE(pipe_compiles<decltype(measure_velocity), decltype(apply_gravity)>);
        REQUIRE_FALSE(pipe_compiles<decltype(physics::particle_count), decltype(apply_gravity)>);
        REQUIRE_FALSE(pipe_compiles<decltype(physics::particle_count), decltype(measure_velocity)>);
    }
}
