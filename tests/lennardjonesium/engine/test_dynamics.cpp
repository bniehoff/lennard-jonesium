/**
 * Test for Dynamics
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/engine/dynamics.hpp>
#include <src/lennardjonesium/physics/system_state.hpp>
#include <src/lennardjonesium/tools/dimensions.hpp>
#include <src/lennardjonesium/tools/cell_list_array.hpp>

#include <tests/lennardjonesium/physics/constant_pairwise_force.hpp>

// In order to test some protected methods, it is useful to define a derived class which makes
// them public
class TestDynamics : public engine::Dynamics
{
    public:
        using engine::Dynamics::Dynamics;
        using engine::Dynamics::impose_boundary_conditions_;
        using engine::Dynamics::rebuild_cell_lists_;
        using engine::Dynamics::compute_forces_;
        using engine::Dynamics::cell_list_array_;
};

SCENARIO("Imposing boundary conditions")
{
    // System of 3 particles
    physics::SystemState state(3);

    // Set the positions
    state.positions = Eigen::Matrix4Xd{
        {1.7,   0,    0},
        {0,   3.2,    0},
        {0,     0, -4.3},
        {0,     0,    0}
    };

    // Set up a box of side length 1.5
    tools::Dimensions dimensions{1.5};

    // Set up a PairwiseForce that represents the absence of forces
    ConstantPairwiseForce pairwise_force{0, 0.5};

    // Set up the Dynamics object
    TestDynamics dynamics(dimensions, pairwise_force);

    WHEN("I impose the boundary conditions on the system")
    {
        dynamics.impose_boundary_conditions_(state);

        THEN("The particle positions are set to the correct locations within the box")
        {
            REQUIRE(Approx(0.2) == state.positions(0, 0));
            REQUIRE(Approx(0.2) == state.positions(1, 1));
            REQUIRE(Approx(0.2) == state.positions(2, 2));
        }
    }
}

SCENARIO("Building/rebuilding the cell lists")
{
    // System of 3 particles
    physics::SystemState state(3);
    state.positions = Eigen::Matrix4Xd{
        {0.3, 0.2,  1.1},
        {1.1, 1.2,  0.2},
        {0.7, 0.1,  0.8},
        {0,     0,    0}
    };

    // Set up the Dynamics object
    tools::Dimensions dimensions{1.5};
    ConstantPairwiseForce pairwise_force{0, 0.5};
    TestDynamics dynamics(dimensions, pairwise_force);

    WHEN("I build the cell lists")
    {
        dynamics.rebuild_cell_lists_(state);

        THEN("I find the particles in the expected cells")
        {
            int particle_count{0};

            for (const auto& cell : dynamics.cell_list_array_.cell_view())
                particle_count += cell.size();
            
            REQUIRE(3 == particle_count);

            REQUIRE(tools::CellList{0} == dynamics.cell_list_array_(0, 2, 1));
            REQUIRE(tools::CellList{1} == dynamics.cell_list_array_(0, 2, 0));
            REQUIRE(tools::CellList{2} == dynamics.cell_list_array_(2, 0, 1));
        }
    }

    WHEN("I move one of the particles")
    {
        state.positions = Eigen::Matrix4Xd{
            {0.3, 0.2,  0.4},
            {1.1, 1.2,  1.4},
            {0.7, 0.1,  0.8},
            {0,     0,    0}
        };

        dynamics.rebuild_cell_lists_(state);

        THEN("I find the particles in the expected cells")
        {
            int particle_count{0};

            for (const auto& cell : dynamics.cell_list_array_.cell_view())
            {
                particle_count += cell.size();
            }
            
            REQUIRE(3 == particle_count);

            REQUIRE(tools::CellList{0, 2} == dynamics.cell_list_array_(0, 2, 1));
            REQUIRE(tools::CellList{1} == dynamics.cell_list_array_(0, 2, 0));
        }
    }
}

SCENARIO("Computing forces between particles")
{
    GIVEN("A state with 2 particles")
    {
        // Let's just use 2 particles
        physics::SystemState state(2);

        // Set up the Dynamics object
        tools::Dimensions dimensions{3.0};
        ConstantPairwiseForce pairwise_force{10.0, 1.0};
        TestDynamics dynamics(dimensions, pairwise_force);

        WHEN("I put the particles near each other in the same cell")
        {
            state.positions = Eigen::Matrix4Xd{
                {0.2, 0.2},
                {0.2, 0.2},
                {0.2, 0.6},
                {  0,   0}
            };

            dynamics.rebuild_cell_lists_(state);
            Eigen::Vector4d force_up{0, 0, 10.0, 0};
            Eigen::Vector4d force_down{0, 0, -10.0, 0};

            THEN("I find the expected forces between them")
            {
                dynamics.compute_forces_(state);

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

            dynamics.rebuild_cell_lists_(state);
            Eigen::Vector4d force_up{0, 0, 10.0, 0};
            Eigen::Vector4d force_down{0, 0, -10.0, 0};

            THEN("I find the expected forces between them")
            {
                dynamics.compute_forces_(state);

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

            dynamics.rebuild_cell_lists_(state);
            Eigen::Vector4d force_up{0, 0, 10.0, 0};
            Eigen::Vector4d force_down{0, 0, -10.0, 0};

            THEN("I find the expected forces between them")
            {
                dynamics.compute_forces_(state);

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

        // Set up the Dynamics object
        tools::Dimensions dimensions{3.0};
        ConstantPairwiseForce pairwise_force{10.0, 1.0};
        TestDynamics dynamics(dimensions, pairwise_force);

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

            dynamics.rebuild_cell_lists_(state);

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
                dynamics.compute_forces_(state);

                REQUIRE(expected_forces.isApprox(state.forces));

                REQUIRE(Approx(expected_potential) == state.potential_energy);
                REQUIRE(Approx(expected_virial) == state.virial);
            }
        }
    }
}
