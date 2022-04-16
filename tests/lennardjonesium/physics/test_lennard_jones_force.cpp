/**
 * Test LennardJonesForce
 */

#include <cmath>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/physics/forces.hpp>
#include <src/lennardjonesium/physics/lennard_jones_force.hpp>

SCENARIO("Lennard-Jones force with cutoff distance")
{
    /**
     * We use a larger-than-usual cutoff distance because then the location of the main features
     * of the potential curve will be close to where we expect:
     * 
     *      nominal_well_loc = location of minimum of potential well, zero slope
     *      nominal_zero_loc = location of where potential crosses x axis
     *      cutoff_distance = should also have zero slope here
     * 
     * This allows us to test that the main features of the potential are behaving correctly.
     */

    double strength = 1.0;
    double cutoff_distance = 4.5;

    double nominal_well_loc = std::pow(2.0, 1.0/6.0);
    double nominal_zero_loc = 1.0;
    double displacement = 0.025;
    double tolerance = 0.002;

    physics::LennardJonesForce lj_force{{strength, cutoff_distance}};

    auto z_vector = Eigen::Vector4d{0, 0, 1, 0};

    WHEN("I check the values close to the origin")
    {
        auto fc = lj_force.compute((nominal_zero_loc - displacement) * z_vector);

        THEN("The potential is positive")
        {
            REQUIRE(fc.potential > 0);
        }

        THEN("The virial is positive")
        {
            REQUIRE(fc.virial > 0);
        }

        THEN("The force is repulsive")
        {
            REQUIRE(fc.force.z() > 0);
        }
    }

    WHEN("I check the values on the near side of the well")
    {
        auto fc = lj_force.compute((nominal_zero_loc + displacement) * z_vector);

        THEN("The potential is negative")
        {
            REQUIRE(fc.potential < 0);
        }

        THEN("The virial is positive")
        {
            REQUIRE(fc.virial > 0);
        }

        THEN("The force is repulsive")
        {
            REQUIRE(fc.force.z() > 0);
        }
    }

    WHEN("I check the values near the nomimal bottom of the well")
    {
        auto fc = lj_force.compute((nominal_well_loc) * z_vector);

        THEN("The potential is close to (minus) the strength value")
        {
            REQUIRE(std::abs(fc.potential + strength) < tolerance);
        }

        THEN("The virial is close to zero")
        {
            REQUIRE(std::abs(fc.virial) < tolerance);
        }

        THEN("The force is close to zero")
        {
            REQUIRE(std::abs(fc.force.z()) < tolerance);
        }
    }

    WHEN("I check the values on the far side of the well")
    {
        auto fc = lj_force.compute((nominal_well_loc + displacement) * z_vector);

        THEN("The potential is negative")
        {
            REQUIRE(fc.potential < 0);
        }

        THEN("The virial is negative")
        {
            REQUIRE(fc.virial < 0);
        }

        THEN("The force is attractive")
        {
            REQUIRE(fc.force.z() < 0);
        }
    }

    WHEN("I check the values near the cutoff distance")
    {
        auto fc = lj_force.compute((cutoff_distance - displacement) * z_vector);

        THEN("The potential is close to zero")
        {
            REQUIRE(std::abs(fc.potential) < tolerance);
        }

        THEN("The virial is close to zero")
        {
            REQUIRE(std::abs(fc.virial) < tolerance);
        }

        THEN("The force is close to zero")
        {
            REQUIRE(std::abs(fc.force.z()) < tolerance);
        }
    }

    WHEN("I check the values beyond the cutoff distance")
    {
        auto fc = lj_force.compute((cutoff_distance + displacement) * z_vector);

        THEN("The potential is zero")
        {
            REQUIRE(fc.potential == 0);
        }

        THEN("The virial is zero")
        {
            REQUIRE(fc.virial == 0);
        }

        THEN("The force is zero")
        {
            REQUIRE(fc.force.z() == 0);
        }
    }
}
