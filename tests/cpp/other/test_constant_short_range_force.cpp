/**
 * Test ConstantShortRangeForce
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/cpp/lennardjonesium/physics/forces.hpp>

#include <tests/cpp/mock/constant_short_range_force.hpp>

SCENARIO("Constant forces with cutoff distance")
{
    GIVEN("A constant attractive force with strength 3.0 and cutoff distance 5.0")
    {
        mock::ConstantShortRangeForce::Parameters parameters = {
            .strength{-3.0}, .cutoff_distance{5.0}
        };

        mock::ConstantShortRangeForce short_range_force{parameters};

        WHEN("I ask for the force at separation distance 2.0")
        {
            double distance = 2.0;

            Eigen::Vector4d separation{0, 0, distance, 0};
            physics::ForceContribution fc = short_range_force.compute(separation);

            THEN("I get the expected values")
            {
                double expected_potential =
                    parameters.strength * (parameters.cutoff_distance - distance);
                double expected_virial = parameters.strength * distance;

                REQUIRE(Eigen::Vector4d{0, 0, parameters.strength, 0} == fc.force);
                REQUIRE(expected_potential == fc.potential);
                REQUIRE(expected_virial == fc.virial);
            }
        }

        WHEN("I ask for the force at separation distance 2.0")
        {
            double distance = 4.0;

            Eigen::Vector4d separation{0, 0, distance, 0};
            physics::ForceContribution fc = short_range_force.compute(separation);

            THEN("I get the expected values")
            {
                double expected_potential =
                    parameters.strength * (parameters.cutoff_distance - distance);
                double expected_virial = parameters.strength * distance;

                REQUIRE(Eigen::Vector4d{0, 0, parameters.strength, 0} == fc.force);
                REQUIRE(expected_potential == fc.potential);
                REQUIRE(expected_virial == fc.virial);
            }
        }

        WHEN("I ask for the force at separation distance 8.0")
        {
            double distance = 8.0;

            Eigen::Vector4d separation{0, 0, distance, 0};
            physics::ForceContribution fc = short_range_force.compute(separation);

            THEN("I get zero")
            {
                REQUIRE(Eigen::Vector4d::Zero() == fc.force);
                REQUIRE(0 == fc.potential);
                REQUIRE(0 == fc.virial);
            }
        }
    }
}
