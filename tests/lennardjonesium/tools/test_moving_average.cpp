/**
 * Test MovingAverage
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/moving_average.hpp>

SCENARIO("Moving averages")
{
    GIVEN("A MovingAverage that keeps 3 values")
    {
        tools::MovingAverage<double> numbers(3);

        WHEN("I enter 2 values")
        {
            numbers.push_back(2);
            numbers.push_back(3);

            THEN("I get the correct sum and average")
            {
                REQUIRE(Approx(5.0) == numbers.sum());
                REQUIRE(Approx(2.5) == numbers.average());
            }
        }

        WHEN("I enter 3 values")
        {
            numbers.push_back(2);
            numbers.push_back(3);
            numbers.push_back(10);

            THEN("I get the correct sum and average")
            {
                REQUIRE(Approx(15.0) == numbers.sum());
                REQUIRE(Approx(5.0) == numbers.average());
            }
        }

        WHEN("I enter 4 values")
        {
            numbers.push_back(2);
            numbers.push_back(3);
            numbers.push_back(8);
            numbers.push_back(1);

            THEN("I get the correct sum and average")
            {
                REQUIRE(Approx(12.0) == numbers.sum());
                REQUIRE(Approx(4.0) == numbers.average());
            }
        }
    }

    GIVEN("A MovingAverage that keeps 3 vectors")
    {
        using vec = Eigen::Vector4d;

        tools::MovingAverage<vec> vectors(3);

        WHEN("I enter 2 values")
        {
            vectors.push_back({1, 0, 0, 0});
            vectors.push_back({0, 1, 0, 0});

            THEN("I get the correct sum and average")
            {
                REQUIRE(vectors.sum().isApprox(vec{1, 1, 0, 0}));
                REQUIRE(vectors.average().isApprox(vec{0.5, 0.5, 0, 0}));
            }
        }

        WHEN("I enter 3 values")
        {
            vectors.push_back({2, 0, 1, 0});
            vectors.push_back({0, 3, 2, 0});
            vectors.push_back({1, 0, 0, 0});

            THEN("I get the correct sum and average")
            {
                REQUIRE(vectors.sum().isApprox(vec{3, 3, 3, 0}));
                REQUIRE(vectors.average().isApprox(vec{1, 1, 1, 0}));
            }
        }

        WHEN("I enter 4 values")
        {
            vectors.push_back({2, 0, 1, 0});
            vectors.push_back({0, 3, 2, 0});
            vectors.push_back({1, 0, 0, 0});
            vectors.push_back({5, 6, 1, 0});

            THEN("I get the correct sum and average")
            {
                REQUIRE(vectors.sum().isApprox(vec{6, 9, 3, 0}));
                REQUIRE(vectors.average().isApprox(vec{2, 3, 1, 0}));
            }
        }
    }
}
