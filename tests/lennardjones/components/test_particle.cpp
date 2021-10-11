/**
 * Test for class Particle
 */

#include <catch2/catch.hpp>
#include <eigen3/Eigen/Dense>

#include <src/lennardjones/tools/AlignedVector3d.hpp>
#include <src/lennardjones/components/particle.hpp>

using components::Particle;

SCENARIO( "Create and assign Particles" ) {
    AlignedVector3d position{1.618, 2.718, 3.142};
    AlignedVector3d displacement{11.618, 12.718, 13.142};
    AlignedVector3d velocity{-3, -4, 2};
    AlignedVector3d acceleration{0, 0, -9.8};

    WHEN( "I create a default Particle" ) {
        Particle p;

        THEN( "Its entries are all zero" ) {
            REQUIRE( p.position.isZero() );
            REQUIRE( p.displacement.isZero() );
            REQUIRE( p.velocity.isZero() );
            REQUIRE( p.acceleration.isZero() );
        }
    }

    WHEN( "I create a Particle with some values" ) {
        Particle q{position, displacement, velocity, acceleration};

        THEN( "Its entries match the values given" ) {
            REQUIRE( position == q.position );
            REQUIRE( displacement == q.displacement );
            REQUIRE( velocity == q.velocity );
            REQUIRE( acceleration == q.acceleration );
        }
    }

    WHEN( "I assign one Particle to another" ) {
        Particle p;
        Particle q{position, displacement, velocity, acceleration};

        p = q;

        THEN( "The Particles have different addresses" ) {
            REQUIRE( &p != &q );
        }

        THEN( "The Particles have the same ID" ) {
            REQUIRE( p.id() == q.id() );
        }

        THEN( "The Particles' values match" ) {
            REQUIRE( q.position == p.position );
            REQUIRE( q.displacement == p.displacement );
            REQUIRE( q.velocity == p.velocity );
            REQUIRE( q.acceleration == p.acceleration );
        }
    }

    WHEN( "I create several Particles" ) {
        Particle::reset_global_id();
        Particle p1, p2, p3, p4, p5;
        Particle q{position, displacement, velocity, acceleration};

        THEN( "They are assigned IDs in sequence starting from zero" ) {
            REQUIRE( 0 == p1.id() );
            REQUIRE( 1 == p2.id() );
            REQUIRE( 2 == p3.id() );
            REQUIRE( 3 == p4.id() );
            REQUIRE( 4 == p5.id() );
            REQUIRE( 5 == q.id() );
        }
    }
}
