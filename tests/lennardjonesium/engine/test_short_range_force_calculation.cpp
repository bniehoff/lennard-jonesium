/**
 * Test ShortRangeForceCalculation
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/bounding_box.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/engine/particle_pair_filter.hpp>
#include <src/lennardjonesium/engine/force_calculation.hpp>

#include <tests/mock/constant_short_range_force.hpp>


SCENARIO("Computing forces between particles")
{
    GIVEN("A state with 2 particles")
    {
        // Let's just use 2 particles
        physics::SystemState state(2);

        // Set up the ShortRangeForceCalculation
        double box_size{3.0};
        double cutoff_distance{1.0};
        double force_strength{-10.0};

        tools::BoundingBox bounding_box{box_size};
        mock::ConstantShortRangeForce short_range_force{force_strength, cutoff_distance};
        engine::CellListParticlePairFilter filter{bounding_box, cutoff_distance};

        engine::ShortRangeForceCalculation force_calculation{short_range_force, filter};

        WHEN("I put the particles near each other in the same cell")
        {
            state.positions = Eigen::Matrix4Xd{
                {0.2, 0.2},
                {0.2, 0.2},
                {0.2, 0.6},
                {  0,   0}
            };

            Eigen::Vector4d force_up{0, 0, 10.0, 0};
            Eigen::Vector4d force_down{0, 0, -10.0, 0};

            THEN("I find the expected forces between them")
            {
                state | force_calculation;

                REQUIRE(force_up.isApprox(state.forces.col(0)));
                REQUIRE(force_down.isApprox(state.forces.col(1)));

                REQUIRE(Approx(-6.0) == state.potential_energy);
                REQUIRE(Approx(-4.0) == state.virial);
            }
        }

        WHEN("I put the particles near each other in adjacent cells")
        {
            state.positions = Eigen::Matrix4Xd{
                {0.2, 0.2},
                {0.2, 0.2},
                {0.8, 1.2},
                {  0,   0}
            };

            Eigen::Vector4d force_up{0, 0, 10.0, 0};
            Eigen::Vector4d force_down{0, 0, -10.0, 0};

            THEN("I find the expected forces between them")
            {
                state | force_calculation;

                REQUIRE(force_up.isApprox(state.forces.col(0)));
                REQUIRE(force_down.isApprox(state.forces.col(1)));

                REQUIRE(Approx(-6.0) == state.potential_energy);
                REQUIRE(Approx(-4.0) == state.virial);
            }
        }

        WHEN("I put the particles near each other across the periodic boundary")
        {
            state.positions = Eigen::Matrix4Xd{
                {0.2, 0.2},
                {0.2, 0.2},
                {2.8, 0.2},
                {  0,   0}
            };

            Eigen::Vector4d force_up{0, 0, 10.0, 0};
            Eigen::Vector4d force_down{0, 0, -10.0, 0};

            THEN("I find the expected forces between them")
            {
                state | force_calculation;

                REQUIRE(force_up.isApprox(state.forces.col(0)));
                REQUIRE(force_down.isApprox(state.forces.col(1)));

                REQUIRE(Approx(-6.0) == state.potential_energy);
                REQUIRE(Approx(-4.0) == state.virial);
            }
        }
    }

    GIVEN("A state with 3 particles")
    {
        // Create state
        physics::SystemState state(3);

        // Set up the ShortRangeForceCalculation
        double box_size{3.0};
        double cutoff_distance{1.0};
        double force_strength{-10.0};

        tools::BoundingBox bounding_box{box_size};
        mock::ConstantShortRangeForce short_range_force{force_strength, cutoff_distance};
        engine::CellListParticlePairFilter filter{bounding_box, cutoff_distance};

        engine::ShortRangeForceCalculation force_calculation{short_range_force, filter};

        WHEN("I put the particles near each other in an L shape")
        {
            /**
             * We put the particles far enough apart that the forces should only act along the
             * horizontal and vertical directions.  The diagonal distance between the last two
             * particles is too far for the force to act.
             */
            state.positions = Eigen::Matrix4Xd{
                {0.6, 1.4, 0.6},
                {0.6, 0.6, 1.4},
                {0.2, 0.2, 0.2},
                {  0,   0,   0}
            };

            Eigen::Matrix4Xd expected_forces{
                {10.0, -10.0,   0.0},
                {10.0,   0.0, -10.0},
                { 0.0,   0.0,   0.0},
                { 0.0,   0.0,   0.0}
            };

            double expected_potential{-4.0};
            double expected_virial{-16.0};

            THEN("I find the expected forces between them")
            {
                state | force_calculation;

                REQUIRE(expected_forces.isApprox(state.forces));

                REQUIRE(Approx(expected_potential) == state.potential_energy);
                REQUIRE(Approx(expected_virial) == state.virial);
            }
        }
    }
}
