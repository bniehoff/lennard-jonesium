/**
 * Test various ParticlePairFilters
 */

#include <iostream>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/bounding_box.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/engine/particle_pair_filter.hpp>
#include <src/lennardjonesium/engine/naive_particle_pair_filter.hpp>
#include <src/lennardjonesium/engine/cell_list_particle_pair_filter.hpp>

SCENARIO("Equality operator of ParticlePair")
{
    GIVEN("Two pairs which have the same entries")
    {
        engine::ParticlePair a{{1, 0, 0, 0}, 3, 4};
        engine::ParticlePair b{{1, 0, 0, 0}, 3, 4};

        THEN("They are equal")
        {
            REQUIRE(a == b);
        }
    }

    GIVEN("Two pairs which have the same entries, but in reverse order")
    {
        engine::ParticlePair a{{1, 0, 0, 0}, 3, 4};
        engine::ParticlePair b{{-1, 0, 0, 0}, 4, 3};

        THEN("They are equal")
        {
            REQUIRE(a == b);
        }
    }

    GIVEN("Two pairs which have the same indices, but different separations")
    {
        engine::ParticlePair a{{1, 0, 0, 0}, 3, 4};
        engine::ParticlePair b{{-1, 0, 0, 0}, 3, 4};

        THEN("They are not equal")
        {
            REQUIRE(a != b);
        }
    }

    GIVEN("Two pairs which have the different indices")
    {
        engine::ParticlePair a{{1, 0, 0, 0}, 3, 4};
        engine::ParticlePair b{{1, 0, 0, 0}, 2, 4};

        THEN("They are not equal")
        {
            REQUIRE(a != b);
        }
    }
}

SCENARIO("Finding ParticlePairs within a cutoff distance")
{
    // We will use a BoundingBox of size 10 and a cutoff of 2
    tools::BoundingBox bounding_box{10.0};
    double cutoff_distance{2.0};

    // Set up a SystemState with particles in various positions
    physics::SystemState state{10};

    state.positions = Eigen::MatrixX4d{
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {1, 9, 9, 0},
        {9, 1, 9, 0},
        {9, 9, 1, 0},
        {5, 5, 5, 0},
        {4, 4, 4, 0},
        {4, 6, 6, 0},
        {6, 4, 6, 0},
        {6, 6, 4, 0},
    }.transpose();

    // Record expected ParticlePairs to be found
    std::vector<engine::ParticlePair> expected_pairs{
        {{-1, -1, -1,  0}, 0, 1},
        {{-1,  1,  1,  0}, 0, 2},
        {{ 1, -1,  1,  0}, 0, 3},
        {{ 1,  1, -1,  0}, 0, 4},
        {{ 1,  1,  1,  0}, 5, 6},
        {{ 1, -1, -1,  0}, 5, 7},
        {{-1,  1, -1,  0}, 5, 8},
        {{-1, -1,  1,  0}, 5, 9}
    };

    WHEN("I look for ParticlePairs using a naive filter")
    {
        engine::NaiveParticlePairFilter filter{bounding_box, cutoff_distance};

        std::vector<engine::ParticlePair> result_pairs;

        for (engine::ParticlePair pair : filter(state))
        {
            result_pairs.push_back(pair);
        }
        
        THEN("I get the expected pairs")
        {
            REQUIRE_THAT(result_pairs, Catch::UnorderedEquals(expected_pairs));
        }
    }

    WHEN("I look for ParticlePairs using a cell list filter")
    {
        engine::CellListParticlePairFilter filter{bounding_box, cutoff_distance};

        std::vector<engine::ParticlePair> result_pairs;

        for (engine::ParticlePair pair : filter(state))
        {
            std::cout << "Received pair" << std::endl;
            result_pairs.push_back(pair);
        }
        
        THEN("I get the expected pairs")
        {
            REQUIRE_THAT(result_pairs, Catch::UnorderedEquals(expected_pairs));
        }
    }
}
