/**
 * Test MovingAverage
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/cpp/lennardjonesium/tools/moving_sample.hpp>

SCENARIO("Computing statistics from moving samples")
{
    GIVEN("A MovingSample that keeps 3 doubles")
    {
        tools::MovingSample<double> numbers(3);

        WHEN("I enter 2 values")
        {
            numbers.push_back(2);
            numbers.push_back(3);

            THEN("I get the correct mean and variance")
            {
                auto s = numbers.statistics();
                REQUIRE(Approx(2.5) == s.mean);
                REQUIRE(Approx(0.5) == s.variance);
            }
        }

        WHEN("I enter 3 values")
        {
            numbers.push_back(2);
            numbers.push_back(3);
            numbers.push_back(10);

            THEN("I get the correct mean and variance")
            {
                auto s = numbers.statistics();
                REQUIRE(Approx(5.0) == s.mean);
                REQUIRE(Approx(19.0) == s.variance);
            }
        }

        WHEN("I enter 4 values")
        {
            numbers.push_back(2);
            numbers.push_back(3);
            numbers.push_back(8);
            numbers.push_back(1);

            THEN("I get the correct mean and variance")
            {
                auto s = numbers.statistics();
                REQUIRE(Approx(4.0) == s.mean);
                REQUIRE(Approx(13.0) == s.variance);
            }
        }
    }

    GIVEN("A MovingAverage that keeps 3 vectors")
    {
        using vec = Eigen::Vector4d;
        using mat = Eigen::Matrix4d;

        tools::MovingSample<vec> vectors(3);

        // double precision = 0.00001;

        WHEN("I sample 2 vectors")
        {
            vectors.push_back({1, 0, 0, 0});
            vectors.push_back({0, 1, 0, 0});

            vec mean{0.5, 0.5, 0, 0};
            mat covariance{
                { 0.5, -0.5,  0.0,  0.0},
                {-0.5,  0.5,  0.0,  0.0},
                { 0.0,  0.0,  0.0,  0.0},
                { 0.0,  0.0,  0.0,  0.0}
            };

            THEN("I get the correct mean and covariance matrix")
            {
                auto s = vectors.statistics();
                REQUIRE(mean.isApprox(s.mean));
                REQUIRE(covariance.isApprox(s.covariance));
            }
        }

        WHEN("I enter 3 values")
        {
            vectors.push_back({2, 0, 1, 0});
            vectors.push_back({0, 3, 2, 0});
            vectors.push_back({1, 0, 0, 0});

            vec mean{1, 1, 1, 0};
            mat covariance{
                { 1.0, -1.5, -0.5,  0.0},
                {-1.5,  3.0,  1.5,  0.0},
                {-0.5,  1.5,  1.0,  0.0},
                { 0.0,  0.0,  0.0,  0.0}
            };

            THEN("I get the correct mean and covariance matrix")
            {
                auto s = vectors.statistics();
                REQUIRE(mean.isApprox(s.mean));
                REQUIRE(covariance.isApprox(s.covariance));
            }
        }

        WHEN("I enter 4 values")
        {
            vectors.push_back({2, 0, 1, 0});
            vectors.push_back({0, 3, 2, 0});
            vectors.push_back({1, 0, 0, 0});
            vectors.push_back({5, 6, 1, 0});

            vec mean{2, 3, 1, 0};
            mat covariance{
                { 7.0,  6.0, -0.5,  0.0},
                { 6.0,  9.0,  1.5,  0.0},
                {-0.5,  1.5,  1.0,  0.0},
                { 0.0,  0.0,  0.0,  0.0}
            };

            THEN("I get the correct mean and covariance matrix")
            {
                auto s = vectors.statistics();
                REQUIRE(mean.isApprox(s.mean));
                REQUIRE(covariance.isApprox(s.covariance));
            }
        }
    }
}
