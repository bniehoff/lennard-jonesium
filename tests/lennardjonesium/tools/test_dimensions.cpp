/**
 * Testing the Dimensions object (very simple)
 */

#include <catch2/catch.hpp>

#include <src/lennardjonesium/tools/dimensions.hpp>

SCENARIO("Constructing Dimensions objects")
{
    double x{1.618};
    double y{2.718};
    double z{3.142};
    double s{1.414};

    WHEN("I create a Dimensions object with three different values")
    {
        auto d = tools::Dimensions(x, y, z);

        THEN("I see the correct values in the member variables")
        {
            REQUIRE(x == d.x);
            REQUIRE(y == d.y);
            REQUIRE(z == d.z);
        }
    }

    WHEN("I create a Dimensions object from a single value")
    {
        auto d = tools::Dimensions(s);

        THEN("I see the same value in all three member variables")
        {
            REQUIRE(s == d.x);
            REQUIRE(s == d.y);
            REQUIRE(s == d.z);
        }
    }
}
