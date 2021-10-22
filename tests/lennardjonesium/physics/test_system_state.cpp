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
