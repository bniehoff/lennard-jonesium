/**
 * Test BoundingBox (very pedantic, just making sure I understand how to use Catch2)
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/bounding_box.hpp>

SCENARIO("Constructing a BoundingBox")
{
    double x{1.618};
    double y{2.718};
    double z{3.142};
    double s{1.414};

    WHEN("I create a BoundingBox with three distinct side lengths")
    {
        tools::BoundingBox box{x, y, z};

        THEN("I get the expected array output")
        {
            REQUIRE(Eigen::Array4d{x, y, z, 1.0}.isApprox(box.array()));
        }

        THEN("I get the expected volume")
        {
            REQUIRE(Approx(x * y * z) == box.volume());
        }
    }

    WHEN("I create a BoundingBox from a single side length")
    {
        tools::BoundingBox box{s};

        THEN("I get the expected array output")
        {
            REQUIRE(Eigen::Array4d{s, s, s, 1.0}.isApprox(box.array()));
        }

        THEN("I get the expected volume")
        {
            REQUIRE(Approx(s * s * s) == box.volume());
        }
    }
}
