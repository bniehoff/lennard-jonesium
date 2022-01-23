/**
 * Test measurements and transformations which compute and change bulk properties.
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/cubic_lattice.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/physics/measurements.hpp>
#include <src/lennardjonesium/physics/transformations.hpp>

SCENARIO("Measurements of bulk properties of the system")
{
    // We set up a state with four particles at the following points
    physics::SystemState state{4};

    state.positions = Eigen::MatrixX4d{
        {0, 0, 0, 0}, {0, 0, 2, 0}, {2, 2, 0, 0}, {2, 2, 2, 0}
    }.transpose();

    // The center of mass of this system is
    Eigen::Vector4d center_of_mass {1, 1, 1, 0};

    // The inertia tensor about the center of mass is
    Eigen::Matrix4d inertia_tensor_about_center {
        { 8, -4,  0,  0},
        {-4,  8,  0,  0},
        { 0,  0,  8,  0},
        { 0,  0,  0, 12}
    };

    // The inertia tensor about the origin is then obtained via the parallel axis theorem
    Eigen::Matrix4d inertia_tensor_about_origin {
        {16, -8, -4,  0},
        {-8, 16, -4,  0},
        {-4, -4, 16,  0},
        { 0,  0,  0, 24}
    };

    THEN("The measured center of mass is correct")
    {
        REQUIRE(center_of_mass.isApprox(physics::center_of_mass(state)));
    }

    THEN("The inertia tensor about the center of mass is correct")
    {
        REQUIRE(inertia_tensor_about_center.isApprox(
            physics::inertia_tensor(state, center_of_mass)
        ));
    }

    THEN("The inertia tensor about the origin is correct")
    {
        REQUIRE(inertia_tensor_about_origin.isApprox(physics::inertia_tensor(state)));
    }

    GIVEN("Some forces on the particles")
    {
        state.forces = Eigen::MatrixX4d{
            {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}
        }.transpose();

        Eigen::Vector4d total_force = Eigen::Vector4d::UnitZ() * 4;

        THEN("The measured total force is correct")
        {
            REQUIRE(total_force.isApprox(physics::total_force(state)));
        }
    }

    GIVEN("Some velocities on the particles rotating around the z axis")
    {
        // Here are the relative coordinates to the center of mass
        //  {-1, -1, -1,  0}, {-1, -1,  1,  0}, { 1,  1, -1,  0}, { 1,  1,  1,  0}
        // and the actual coordinates
        //  { 0,  0,  0,  0}, { 0,  0,  2,  0}, { 2,  2,  0,  0}, { 2,  2,  2,  0}

        state.velocities = Eigen::MatrixX4d{
            { 1, -1,  0,  0}, { 1, -1,  0,  0}, {-1,  1,  0,  0}, {-1,  1,  0,  0}
        }.transpose();

        double kinetic_energy{4};
        double temperature{8./3.};

        Eigen::Vector4d total_momentum{0, 0, 0, 0};

        Eigen::Vector4d angular_momentum_about_center{0, 0, 8, 0};
        Eigen::Vector4d angular_momentum_about_origin{0, 0, 8, 0};

        THEN("The measured kinetic energy and temperature are correct")
        {
            REQUIRE(Approx(kinetic_energy) == physics::kinetic_energy(state));
            REQUIRE(Approx(temperature) == physics::temperature(state));
        }

        THEN("The measured total momentum is correct")
        {
            REQUIRE(total_momentum.isApprox(physics::total_momentum(state)));
        }

        THEN("The measured angular momentum is correct")
        {
            REQUIRE(angular_momentum_about_center.isApprox(
                physics::total_angular_momentum(state, center_of_mass)
            ));

            REQUIRE(angular_momentum_about_origin.isApprox(
                physics::total_angular_momentum(state)
            ));
        }
    }

    GIVEN("Some velocities on the particles rotating around the x axis")
    {
        // Here are the relative coordinates to the center of mass
        //  {-1, -1, -1,  0}, {-1, -1,  1,  0}, { 1,  1, -1,  0}, { 1,  1,  1,  0}
        // and the actual coordinates
        //  { 0,  0,  0,  0}, { 0,  0,  2,  0}, { 2,  2,  0,  0}, { 2,  2,  2,  0}

        state.velocities = Eigen::MatrixX4d{
            { 0, -1,  1,  0}, { 0,  1,  1,  0}, { 0, -1, -1,  0}, { 0,  1, -1,  0}
        }.transpose();

        double kinetic_energy{4};
        double temperature{8./3.};

        Eigen::Vector4d total_momentum{0, 0, 0, 0};

        /**
         * Cross products r x p for relative r
         * 
         *      {-2,  1,  1,  0}
         *      {-2,  1, -1,  0}
         *      {-2,  1, -1,  0}
         *      {-2,  1,  1,  0}
         */
        Eigen::Vector4d angular_momentum_about_center{-8, 4, 0, 0};

        /**
         * Cross products r x p for absolute r
         * 
         *      { 0,  0,  0,  0}
         *      {-2,  0,  0,  0}
         *      {-2,  2, -2,  0}
         *      {-4,  2,  2,  0}
         */
        Eigen::Vector4d angular_momentum_about_origin{-8, 4, 0, 0};

        THEN("The measured kinetic energy and temperature are correct")
        {
            REQUIRE(Approx(kinetic_energy) == physics::kinetic_energy(state));
            REQUIRE(Approx(temperature) == physics::temperature(state));
        }

        THEN("The measured total momentum is correct")
        {
            REQUIRE(total_momentum.isApprox(physics::total_momentum(state)));
        }

        THEN("The measured angular momentum is correct")
        {
            REQUIRE(angular_momentum_about_center.isApprox(
                physics::total_angular_momentum(state, center_of_mass)
            ));

            REQUIRE(angular_momentum_about_origin.isApprox(
                physics::total_angular_momentum(state)
            ));
        }
    }
}

SCENARIO("Transformations of bulk properties of a small system")
{
    // We set up a state with four particles at the following points
    physics::SystemState state{4};

    state.positions = Eigen::MatrixX4d{
        {0, 0, 0, 0}, {0, 0, 2, 0}, {2, 2, 0, 0}, {2, 2, 2, 0}
    }.transpose();

    // The center of mass of this system is
    Eigen::Vector4d center_of_mass {1, 1, 1, 0};

    // The inertia tensor about the center of mass is
    Eigen::Matrix4d inertia_tensor_about_center {
        { 8, -4,  0,  0},
        {-4,  8,  0,  0},
        { 0,  0,  8,  0},
        { 0,  0,  0, 12}
    };

    // The inertia tensor about the origin is then obtained via the parallel axis theorem
    Eigen::Matrix4d inertia_tensor_about_origin {
        {16, -8, -4,  0},
        {-8, 16, -4,  0},
        {-4, -4, 16,  0},
        { 0,  0,  0, 24}
    };

    GIVEN("Some velocities on the particles rotating around the z axis")
    {
        // Here are the relative coordinates to the center of mass
        //  {-1, -1, -1,  0}, {-1, -1,  1,  0}, { 1,  1, -1,  0}, { 1,  1,  1,  0}
        // and the actual coordinates
        //  { 0,  0,  0,  0}, { 0,  0,  2,  0}, { 2,  2,  0,  0}, { 2,  2,  2,  0}

        state.velocities = Eigen::MatrixX4d{
            { 1, -1,  0,  0}, { 1, -1,  0,  0}, {-1,  1,  0,  0}, {-1,  1,  0,  0}
        }.transpose();

        Eigen::Vector4d total_momentum{0, 0, 0, 0};

        Eigen::Vector4d angular_momentum_about_center{0, 0, 8, 0};
        Eigen::Vector4d angular_momentum_about_origin{0, 0, 8, 0};

        WHEN("I set the momentum to a new value")
        {
            Eigen::Vector4d new_momentum{1.618, 2.718, 3.142, 0};

            state | physics::set_momentum(new_momentum);

            THEN("I get the correct result")
            {
                REQUIRE(new_momentum.isApprox(physics::total_momentum(state)));
            }
        }

        WHEN("I set the temperature to a new value")
        {
            double new_temperature{0.5};

            state | physics::set_temperature(new_temperature);

            THEN("I get the correct result")
            {
                REQUIRE(Approx(new_temperature) == physics::temperature(state));
            }
        }

        WHEN("I set the angular momentum to a new value")
        {
            Eigen::Vector4d new_angular_momentum{1.618, 2.718, 3.142, 0};

            state | physics::set_angular_momentum(new_angular_momentum);

            THEN("I get the correct result")
            {
                REQUIRE(new_angular_momentum.isApprox(physics::total_angular_momentum(state)));
            }
        }
    }

    GIVEN("Some velocities on the particles rotating around the x axis")
    {
        // Here are the relative coordinates to the center of mass
        //  {-1, -1, -1,  0}, {-1, -1,  1,  0}, { 1,  1, -1,  0}, { 1,  1,  1,  0}
        // and the actual coordinates
        //  { 0,  0,  0,  0}, { 0,  0,  2,  0}, { 2,  2,  0,  0}, { 2,  2,  2,  0}

        state.velocities = Eigen::MatrixX4d{
            { 0, -1,  1,  0}, { 0,  1,  1,  0}, { 0, -1, -1,  0}, { 0,  1, -1,  0}
        }.transpose();

        Eigen::Vector4d total_momentum{0, 0, 0, 0};

        /**
         * Cross products r x p for relative r
         * 
         *      {-2,  1,  1,  0}
         *      {-2,  1, -1,  0}
         *      {-2,  1, -1,  0}
         *      {-2,  1,  1,  0}
         */
        Eigen::Vector4d angular_momentum_about_center{-8, 4, 0, 0};

        /**
         * Cross products r x p for absolute r
         * 
         *      { 0,  0,  0,  0}
         *      {-2,  0,  0,  0}
         *      {-2,  2, -2,  0}
         *      {-4,  2,  2,  0}
         */
        Eigen::Vector4d angular_momentum_about_origin{-8, 4, 0, 0};

        WHEN("I set the momentum to a new value")
        {
            Eigen::Vector4d new_momentum{1.618, 2.718, 3.142, 0};

            state | physics::set_momentum(new_momentum);

            THEN("I get the correct result")
            {
                REQUIRE(new_momentum.isApprox(physics::total_momentum(state)));
            }
        }

        WHEN("I set the temperature to a new value")
        {
            double new_temperature{0.5};

            state | physics::set_temperature(new_temperature);

            THEN("I get the correct result")
            {
                REQUIRE(Approx(new_temperature) == physics::temperature(state));
            }
        }

        WHEN("I set the angular momentum to a new value")
        {
            Eigen::Vector4d new_angular_momentum{1.618, 2.718, 3.142, 0};

            state | physics::set_angular_momentum(new_angular_momentum);

            THEN("I get the correct result")
            {
                REQUIRE(new_angular_momentum.isApprox(physics::total_angular_momentum(state)));
            }
        }
    }

    GIVEN("Some random velocities on the particles")
    {
        state.velocities.setRandom();

        WHEN("I set the momentum to a new value")
        {
            Eigen::Vector4d new_momentum{1.618, 2.718, 3.142, 0};

            state | physics::set_momentum(new_momentum);

            THEN("I get the correct result")
            {
                REQUIRE(new_momentum.isApprox(physics::total_momentum(state)));
            }
        }

        WHEN("I set the temperature to a new value")
        {
            double new_temperature{0.5};

            state | physics::set_temperature(new_temperature);

            THEN("I get the correct result")
            {
                REQUIRE(Approx(new_temperature) == physics::temperature(state));
            }
        }

        WHEN("I set the angular momentum to a new value")
        {
            Eigen::Vector4d new_angular_momentum{1.618, 2.718, 3.142, 0};

            state | physics::set_angular_momentum(new_angular_momentum);

            THEN("I get the correct result")
            {
                REQUIRE(new_angular_momentum.isApprox(physics::total_angular_momentum(state)));
            }
        }
    }
}

SCENARIO("Transformations of bulk properties on a larger random state")
{
    // We set up a system state with several particles in a crystal lattice structure
    int particle_count{50};
    double density{1.0};

    tools::CubicLattice cubic_lattice{particle_count, density, tools::CubicLattice::BodyCentered()};
    physics::SystemState state{particle_count};

    for (int i = 0; auto position : cubic_lattice())
    {
        state.positions.col(i) = position;
        ++i;
    }

    GIVEN("Some random velocities on the particles")
    {
        state.velocities.setRandom();

        WHEN("I set the momentum to a new value")
        {
            Eigen::Vector4d new_momentum{1.618, 2.718, 3.142, 0};

            state | physics::set_momentum(new_momentum);

            THEN("I get the correct result")
            {
                REQUIRE(new_momentum.isApprox(physics::total_momentum(state)));
            }
        }

        WHEN("I set the temperature to a new value")
        {
            double new_temperature{0.5};

            state | physics::set_temperature(new_temperature);

            THEN("I get the correct result")
            {
                REQUIRE(Approx(new_temperature) == physics::temperature(state));
            }
        }

        WHEN("I set the angular momentum to a new value")
        {
            Eigen::Vector4d new_angular_momentum{1.618, 2.718, 3.142, 0};

            state | physics::set_angular_momentum(new_angular_momentum);

            THEN("I get the correct result")
            {
                REQUIRE(new_angular_momentum.isApprox(physics::total_angular_momentum(state)));
            }
        }
    }
}
