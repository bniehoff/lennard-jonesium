/**
 * Test CubicLattice
 */

#include <vector>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/cpp/lennardjonesium/tools/bounding_box.hpp>
#include <src/cpp/lennardjonesium/tools/system_parameters.hpp>
#include <src/cpp/lennardjonesium/tools/cubic_lattice.hpp>

SCENARIO("Producing particle lattices with the correct density")
{
    tools::SystemParameters system_parameters{
        .temperature{0.5},
        .density{2.718},
        .particle_count{7}
    };

    WHEN("I create a simple cubic lattice")
    {
        tools::CubicLattice lattice(
            system_parameters, tools::CubicLattice::Simple()
        );
        tools::BoundingBox bounding_box = lattice.bounding_box();
        double density = (
            static_cast<double>(system_parameters.particle_count) / bounding_box.volume()
        );

        THEN("The bounding box is a cube")
        {
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().y());
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().z());
        }

        THEN("I get a lattice of the desired density")
        {
            REQUIRE(Approx(system_parameters.density) == density);
        }
    }

    WHEN("I create a body-centered cubic lattice")
    {
        tools::CubicLattice lattice(
            system_parameters, tools::CubicLattice::BodyCentered()
        );
        tools::BoundingBox bounding_box = lattice.bounding_box();
        double density = (
            static_cast<double>(system_parameters.particle_count) / bounding_box.volume()
        );

        THEN("The bounding box is a cube")
        {
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().y());
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().z());
        }

        THEN("I get a lattice of the desired density")
        {
            REQUIRE(Approx(system_parameters.density) == density);
        }
    }

    WHEN("I create a face-centered cubic lattice")
    {
        tools::CubicLattice lattice(
            system_parameters, tools::CubicLattice::FaceCentered()
        );
        tools::BoundingBox bounding_box = lattice.bounding_box();
        double density = (
            static_cast<double>(system_parameters.particle_count) / bounding_box.volume()
        );

        THEN("The bounding box is a cube")
        {
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().y());
            REQUIRE(Approx(bounding_box.array().x()) == bounding_box.array().z());
        }

        THEN("I get a lattice of the desired density")
        {
            REQUIRE(Approx(system_parameters.density) == density);
        }
    }
}

SCENARIO("Producing particle lattices with the correct lattice sites")
{
    tools::SystemParameters system_parameters{
        .temperature{0.5},
        .density{7.0 / 8.0},
        .particle_count{7}
    };

    WHEN("I create a simple cubic lattice")
    {
        tools::CubicLattice lattice(
            system_parameters, tools::CubicLattice::Simple()
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
            system_parameters, tools::CubicLattice::BodyCentered()
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
            system_parameters, tools::CubicLattice::FaceCentered()
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
