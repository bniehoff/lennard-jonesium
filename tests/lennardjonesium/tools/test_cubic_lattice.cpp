/**
 * Test CubicLattice
 */

#include <vector>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/bounding_box.hpp>
#include <src/lennardjonesium/tools/cubic_lattice.hpp>

SCENARIO("Producing particle lattices with the correct density")
{
    int particle_count{7};
    double desired_density{2.718};

    WHEN("I create a simple cubic lattice")
    {
        tools::CubicLattice lattice(
            particle_count, desired_density, tools::CubicLattice::Simple()
        );
        tools::BoundingBox bounding_box = lattice.bounding_box();
        double density = static_cast<double>(particle_count) / bounding_box.volume();

        THEN("The bounding box is a cube")
        {
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().y());
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().z());
        }

        THEN("I get a lattice of the desired density")
        {
            REQUIRE(Approx(desired_density) == density);
        }
    }

    WHEN("I create a body-centered cubic lattice")
    {
        tools::CubicLattice lattice(
            particle_count, desired_density, tools::CubicLattice::BodyCentered()
        );
        tools::BoundingBox bounding_box = lattice.bounding_box();
        double density = static_cast<double>(particle_count) / bounding_box.volume();

        THEN("The bounding box is a cube")
        {
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().y());
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().z());
        }

        THEN("I get a lattice of the desired density")
        {
            REQUIRE(Approx(desired_density) == density);
        }
    }

    WHEN("I create a face-centered cubic lattice")
    {
        tools::CubicLattice lattice(
            particle_count, desired_density, tools::CubicLattice::FaceCentered()
        );
        tools::BoundingBox bounding_box = lattice.bounding_box();
        double density = static_cast<double>(particle_count) / bounding_box.volume();

        THEN("The bounding box is a cube")
        {
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().y());
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().z());
        }

        THEN("I get a lattice of the desired density")
        {
            REQUIRE(Approx(desired_density) == density);
        }
    }
}

SCENARIO("Producing particle lattices with the correct lattice sites")
{
    int particle_count{7};
    double desired_density{7.0 / 8.0};

    WHEN("I create a simple cubic lattice")
    {
        tools::CubicLattice lattice(
            particle_count, desired_density, tools::CubicLattice::Simple()
        );

        std::vector<Eigen::Vector4d> expected_sites{
            {0, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 0, 0}, {0, 1, 1, 0},
            {1, 0, 0, 0}, {1, 0, 1, 0}, {1, 1, 0, 0}
        };

        std::vector<Eigen::Vector4d> result_sites{};
        for (auto site : lattice())
        {
            result_sites.push_back(site);
        }

        THEN("I get the correct lattice sites")
        {
            REQUIRE_THAT(result_sites, Catch::UnorderedEquals(expected_sites));
        }
    }

    WHEN("I create a body-centered cubic lattice")
    {
        tools::CubicLattice lattice(
            particle_count, desired_density, tools::CubicLattice::BodyCentered()
        );

        std::vector<Eigen::Vector4d> expected_sites{
            {0, 0, 0, 0}, {0.5, 0.5, 0.5, 0}, {0, 0, 1, 0}, {0.5, 0.5, 1.5, 0},
            {0, 1, 0, 0}, {0.5, 1.5, 0.5, 0}, {0, 1, 1, 0}
        };

        std::vector<Eigen::Vector4d> result_sites{};
        for (auto site : lattice())
        {
            result_sites.push_back(site);
        }

        THEN("I get the correct lattice sites")
        {
            REQUIRE_THAT(result_sites, Catch::UnorderedEquals(expected_sites));
        }
    }

    WHEN("I create a face-centered cubic lattice")
    {
        tools::CubicLattice lattice(
            particle_count, desired_density, tools::CubicLattice::FaceCentered()
        );

        std::vector<Eigen::Vector4d> expected_sites{
            {0, 0, 0, 0}, {0.5, 0.5, 0, 0}, {0.5, 0, 0.5, 0}, {0, 0.5, 0.5, 0},
            {0, 0, 1, 0}, {0.5, 0.5, 1, 0}, {0.5, 0, 1.5, 0}
        };

        std::vector<Eigen::Vector4d> result_sites{};
        for (auto site : lattice())
        {
            result_sites.push_back(site);
        }

        THEN("I get the correct lattice sites")
        {
            REQUIRE_THAT(result_sites, Catch::UnorderedEquals(expected_sites));
        }
    }
}
