/**
 * A fairly trivial test, since SystemState is just data.
 */

#include <ranges>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/engine/system_state.hpp>

using Eigen::Matrix4Xd;
using Eigen::Vector4d;

using engine::SystemState;

SCENARIO( "Representing the system state" ) {
    WHEN( "I create a SystemState for a given number of particles" ) {
        SystemState s(7);

        THEN( "It has the expected number of particles" ) {
            REQUIRE( 7 == s.positions.cols() );
            REQUIRE( 7 == s.velocities.cols() );
            REQUIRE( 7 == s.forces.cols() );
            REQUIRE( 7 == s.displacements.cols() );
        }
    }

    WHEN( "I set the number of particles at runtime" ) {
        SystemState s;

        s.set_particle_count(7);

        THEN( "It has the expected number of particles" ) {
            REQUIRE( 7 == s.positions.cols() );
            REQUIRE( 7 == s.velocities.cols() );
            REQUIRE( 7 == s.forces.cols() );
            REQUIRE( 7 == s.displacements.cols() );
        }
    }

    WHEN( "I set some of the column vectors" ) {
        int count = 7;
        SystemState s(count);

        for (const auto i : std::views::iota(0, count)) {
            s.positions.col(i) = Vector4d{i + 0.0, i + 1.0, i + 2.0, i + 3.0};
        }

        THEN( "I can read back the same vectors" ) {
            REQUIRE( Vector4d{3, 4, 5, 6} == s.positions.col(3) );
        }
    }
}

SCENARIO( "Piping operations" ) {
    SystemState s(1);
    Vector4d gravity{0, 0, -10, 0};

    auto increase_velocity = [](SystemState& s) -> SystemState& {
        s.velocities.col(0) += Vector4d::UnitZ();
        return s;
    };

    auto apply_gravity = [gravity](SystemState& s) -> SystemState& {
        s.velocities.col(0) += gravity;
        return s;
    };

    WHEN( "I apply a chain of functions to the state" ) {
        s | increase_velocity | increase_velocity | apply_gravity;

        THEN( "I get the expected result" ) {
            REQUIRE( Vector4d{0, 0, -8, 0} == s.velocities.col(0) );
        }
    }
}
