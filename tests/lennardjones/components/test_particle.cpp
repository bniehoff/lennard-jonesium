/**
 * Test for class Particle
 */

// #include <ranges>
#include <list>
#include <array>
#include <ranges>

#include <catch2/catch.hpp>
#include <eigen3/Eigen/Dense>

#include <src/lennardjones/tools/aligned_vectors.hpp>
#include <src/lennardjones/components/particle.hpp>

using components::Particle;

SCENARIO( "Create and assign Particles" ) {
    AlignedVector3d position{1.618, 2.718, 3.142};
    AlignedVector3d velocity{-3, -4, 2};
    AlignedVector3d acceleration{0, 0, -9.8};

    WHEN( "I create a default Particle" ) {
        Particle p;

        THEN( "Its entries are all zero" ) {
            REQUIRE( p.position().isZero() );
            REQUIRE( p.displacement().isZero() );
            REQUIRE( p.velocity().isZero() );
            REQUIRE( p.acceleration().isZero() );
        }
    }

    WHEN( "I create a Particle with position and velocity" ) {
        Particle q{position, velocity};

        THEN( "The position and velocity match the values given" ) {
            REQUIRE( position == q.position() );
            REQUIRE( velocity == q.velocity() );
        }

        THEN( "The acceleration and displacement are zero" ) {
            REQUIRE( q.acceleration().isZero() );
            REQUIRE( q.displacement().isZero() );
        }
    }

    WHEN( "I create a Particle with position, velocity, and acceleration" ) {
        Particle q{position, velocity, acceleration};

        THEN( "The position, velocity, and acceleration match the values given" ) {
            REQUIRE( position     == q.position() );
            REQUIRE( velocity     == q.velocity() );
            REQUIRE( acceleration == q.acceleration() );
        }

        THEN( "The displacement is zero" ) {
            REQUIRE( q.displacement().isZero() );
        }
    }

    WHEN( "I assign one Particle to another" ) {
        Particle p;
        Particle q{position, velocity};

        p = q;

        THEN( "The Particles have different addresses" ) {
            REQUIRE( &p != &q );
        }

        THEN( "The Particles have the same ID" ) {
            REQUIRE( p.id() == q.id() );
        }

        THEN( "The Particles' values match" ) {
            REQUIRE( q.position()     == p.position()     );
            REQUIRE( q.displacement() == p.displacement() );
            REQUIRE( q.velocity()     == p.velocity()     );
            REQUIRE( q.acceleration() == p.acceleration() );
        }
    }

    WHEN( "I create several Particles" ) {
        Particle::reset_global_id();
        Particle p1, p2, p3, p4, p5;
        Particle q{position, velocity};

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

/**
 * This "test" is really just for myself to understand how to use std::list<Particle*>
 */
SCENARIO( "Lists and move semantics" ) {
    // First create a global vector of 10 particles by id
    const int particle_count{10};

    // Need to make sure the Particles are numbered 0 through 9 every time the Scenario resets
    Particle::reset_global_id();
    std::array<Particle, particle_count> master_array;

    WHEN( "I check the ID of each particle" ) {
        THEN( "It matches its index in the master list" ) {
            for(auto i : std::views::iota(0, particle_count)) {
                REQUIRE( i == master_array[i].id() );
            }
        }
    }

    WHEN( "I add some particles to a list" ) {
        std::list<Particle*> cell_list;
        std::vector<int> indices{3, 1, 9, 6, 7};

        for(auto i : indices) {
            cell_list.push_back(&master_array[i]);
        }

        THEN( "Their IDs refer to their positions in the master list" ) {
            for(auto p : cell_list) {
                REQUIRE( p == &master_array[p->id()] );
            }
        }
    }
}
