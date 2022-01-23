/**
 * Test various measurement functions.
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/measurements.hpp>

SCENARIO("Measuring bulk properties of the state")
{
    GIVEN("A state with three particles and internal forces")
    {
        physics::SystemState state{3};

        state.positions = Eigen::MatrixX4d{
            { 0.0,  0.0,  7.0,  0.0},
            { 3.0,  0.0,  0.0,  0.0},
            {-3.0,  0.0,  0.0,  0.0}
        }.transpose();

        state.velocities = Eigen::MatrixX4d{
            { 0.0,  0.0,  2.0,  0.0},
            { 0.0,  5.0,  0.0,  0.0},
            { 0.0, -5.0,  0.0,  0.0}
        }.transpose();

        state.forces = Eigen::MatrixX4d{
            { 0.0,  0.0,  0.0,  0.0},
            {-8.0,  0.0,  0.0,  0.0},
            { 8.0,  0.0,  0.0,  0.0}
        }.transpose();

        WHEN("I measure the kinetic energy")
        {
            double kinetic_energy = 54.0 / 2.0;

            THEN("I get the expected result")
            {
                REQUIRE(Approx(kinetic_energy) == physics::kinetic_energy(state));
            }
        }

        WHEN("I measure the total momentum")
        {
            Eigen::Vector4d total_momentum{0.0, 0.0, 2.0, 0.0};

            THEN("I get the expected result")
            {
                REQUIRE(total_momentum.isApprox(physics::total_momentum(state)));
            }
        }

        WHEN("I measure the total force")
        {
            Eigen::Vector4d total_force{0.0, 0.0, 0.0, 0.0};

            THEN("I get the expected result")
            {
                REQUIRE(total_force.isApprox(physics::total_force(state)));
            }
        }

        WHEN("I measure the center of mass")
        {
            Eigen::Vector4d center_of_mass{0.0, 0.0, 7.0 / 3.0, 0.0};

            THEN("I get the expected result")
            {
                REQUIRE(center_of_mass.isApprox(physics::center_of_mass(state)));
            }
        }

        WHEN("I measure the total angular momentum")
        {
            Eigen::Vector4d total_angular_momentum{0.0, 0.0, 15.0 * 2.0, 0.0};

            THEN("I get the expected result")
            {
                REQUIRE(total_angular_momentum.isApprox(physics::total_angular_momentum(state)));
            }
        }

        WHEN("I measure the inertia tensor")
        {
            Eigen::Matrix4d inertia_tensor{
                {49.0,   0.0,   0.0,   0.0},
                { 0.0,  67.0,   0.0,   0.0},
                { 0.0,   0.0,  18.0,   0.0},
                { 0.0,   0.0,   0.0,  67.0}
            };

            THEN("I get the expected result")
            {
                REQUIRE(inertia_tensor.isApprox(physics::inertia_tensor(state)));
            }
        }
    }
}
