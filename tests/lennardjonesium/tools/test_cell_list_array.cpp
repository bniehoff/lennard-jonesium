/**
 * Test CellListArray
 */

#include <vector>
#include <algorithm>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/bounding_box.hpp>
#include <src/lennardjonesium/tools/cell_list_array.hpp>

SCENARIO("Creating a CellListArray and accessing elements")
{
    tools::BoundingBox bounding_box{1.0};

    WHEN("I create a CellListArray with 0.3 cutoff")
    {
        double cutoff_distance{0.3};

        tools::CellListArray cell_list_array{bounding_box, cutoff_distance};

        THEN("It has 3x3x3 elements")
        {
            REQUIRE((Eigen::Array4i{3, 3, 3, 0} == cell_list_array.shape()).all());
        }

        THEN("I can access the elements as expected")
        {
            tools::CellList entry{1, 3, 4, 6, 7, 9};

            cell_list_array(2, 1, 1) = entry;

            REQUIRE(entry == cell_list_array(2, 1, 1));
        }
    }
}

SCENARIO("Using the cells() generator on a 3x3x3 array")
{
    tools::BoundingBox bounding_box{1.0};
    double cutoff_distance{0.3};
    tools::CellListArray cell_list_array{bounding_box, cutoff_distance};

    WHEN("I iterate over the cells in a 3x3x3 array")
    {
        int count{0};

        for ([[maybe_unused]] const auto& cell : cell_list_array.cells()) {
            count++;
        }

        THEN("I get the correct number of cells")
        {
            REQUIRE(27 == count);
        }
    }
}

void setup_cell_list_array(tools::CellListArray& cell_list_array)
{
    // To set up, let's populate the cell list array with its own index (as an array), so that we
    // can easily see which cells have been selected
    for (int i = 0; i < cell_list_array.shape()[0]; i++)
        for (int j = 0; j < cell_list_array.shape()[1]; j++)
            for (int k = 0; k < cell_list_array.shape()[2]; k++)
                cell_list_array(i, j, k) = tools::CellList{i, j, k};
}

SCENARIO("Clearing the array") {
    GIVEN("A 3x3x3 array set up with its own indices") {
        tools::BoundingBox bounding_box{1.0};
        double cutoff_distance{0.3};

        tools::CellListArray cell_list_array{bounding_box, cutoff_distance};
        setup_cell_list_array(cell_list_array);

        WHEN("I clear the array") {
            cell_list_array.clear();

            THEN("Every cell is empty") {
                for (const auto& cell : cell_list_array.cells())
                    REQUIRE(cell.empty());
            }
        }
    }
}

/**
 * To test the adjacent_pairs() generator, we need some helping functions.  The idea is to
 * encode the "difference" between any two cells into a single integer.  We can later check whether
 * all distinct possible integers were seen exactly once.
 */

// This function will be useful in the following tests:
struct NeighborCodes {int first; int second;};

NeighborCodes get_neighbor_codes(tools::CellListPair pair, tools::CellListArray cla)
{
    // We assume that the cell lists have been set up to store their own indices using
    // setup_cell_list_array() above
    int displacement[3];
    
    for (int d = 0; d < 3; d++)
        displacement[d] = pair.second[d] - pair.first[d] + pair.lattice_image[d] * cla.shape()[d];
    
    // Each possible displacement is then mapped to a number from 0 to 26.
    // {0, 0, 0} is mapped to 13.
    return NeighborCodes {
        9 * (1 + displacement[0]) + 3 * (1 + displacement[1]) + (1 + displacement[2]),
        9 * (1 - displacement[0]) + 3 * (1 - displacement[1]) + (1 - displacement[2])
    };
}

SCENARIO("Using the adjacent_pairs() generator various-sized arrays")
{
    // The list of neighbor codes should contain every entry from 0 to 26, except 13
    const tools::CellList all_neighbor_codes {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26
    };

    GIVEN("A 3x3x3 array set up with its own indices")
    {
        tools::BoundingBox bounding_box{1.0};
        double cutoff_distance{0.3};

        tools::CellListArray cell_list_array{bounding_box, cutoff_distance};
        setup_cell_list_array(cell_list_array);

        // Separate result array, since the elements of a NeighborPair are const
        tools::CellListArray neighbor_codes_array(bounding_box, cutoff_distance);

        WHEN("I append neighbor codes to all neighbors in pairs")
        {
            for (auto neighbor_pair : cell_list_array.adjacent_pairs())
            {
                auto neighbor_codes = get_neighbor_codes(neighbor_pair, cell_list_array);

                neighbor_codes_array(
                    neighbor_pair.first[0], neighbor_pair.first[1], neighbor_pair.first[2]
                ).push_back(neighbor_codes.first);

                neighbor_codes_array(
                    neighbor_pair.second[0], neighbor_pair.second[1], neighbor_pair.second[2]
                ).push_back(neighbor_codes.second);
            }

            THEN("Every cell has interacted with each of its 26 neighbors exactly once")
            {
                for (const auto& neighbor_code_list : neighbor_codes_array.cells())
                {
                    // Need a mutable copy to be able to sort it
                    tools::CellList temp{neighbor_code_list};

                    std::sort(temp.begin(), temp.end());
                    REQUIRE(all_neighbor_codes == temp);
                }
            }
        }
    }

    GIVEN("A 2x2x2 array set up with its own indices")
    {
        tools::BoundingBox bounding_box{1.0};
        double cutoff_distance{0.48};

        tools::CellListArray cell_list_array{bounding_box, cutoff_distance};
        setup_cell_list_array(cell_list_array);

        // Separate result array, since the elements of a NeighborPair are const
        tools::CellListArray neighbor_codes_array(bounding_box, cutoff_distance);

        WHEN("I append neighbor codes to all neighbors in pairs")
        {
            for (auto neighbor_pair : cell_list_array.adjacent_pairs())
            {
                auto neighbor_codes = get_neighbor_codes(neighbor_pair, cell_list_array);

                neighbor_codes_array(
                    neighbor_pair.first[0], neighbor_pair.first[1], neighbor_pair.first[2]
                ).push_back(neighbor_codes.first);

                neighbor_codes_array(
                    neighbor_pair.second[0], neighbor_pair.second[1], neighbor_pair.second[2]
                ).push_back(neighbor_codes.second);
            }

            THEN("Every cell has interacted with each of its 26 neighbors exactly once")
            {
                for (const auto& neighbor_code_list : neighbor_codes_array.cells())
                {
                    // Need a mutable copy to be able to sort it
                    tools::CellList temp = neighbor_code_list;

                    std::sort(temp.begin(), temp.end());
                    REQUIRE(all_neighbor_codes == temp);
                }
            }
        }
    }

    GIVEN("A 1x1x1 array set up with its own indices")
    {
        tools::BoundingBox bounding_box{1.0};
        double cutoff_distance{0.67};

        tools::CellListArray cell_list_array{bounding_box, cutoff_distance};
        setup_cell_list_array(cell_list_array);

        // Separate result array, since the elements of a NeighborPair are const
        tools::CellListArray neighbor_codes_array(bounding_box, cutoff_distance);

        WHEN("I append neighbor codes to all neighbors in pairs")
        {
            for (auto neighbor_pair : cell_list_array.adjacent_pairs())
            {
                auto neighbor_codes = get_neighbor_codes(neighbor_pair, cell_list_array);

                neighbor_codes_array(
                    neighbor_pair.first[0], neighbor_pair.first[1], neighbor_pair.first[2]
                ).push_back(neighbor_codes.first);

                neighbor_codes_array(
                    neighbor_pair.second[0], neighbor_pair.second[1], neighbor_pair.second[2]
                ).push_back(neighbor_codes.second);
            }

            THEN("Every cell has interacted with each of its 26 neighbors exactly once")
            {
                for (const auto& neighbor_code_list : neighbor_codes_array.cells())
                {
                    // Need a mutable copy to be able to sort it
                    tools::CellList temp = neighbor_code_list;

                    std::sort(temp.begin(), temp.end());
                    REQUIRE(all_neighbor_codes == temp);
                }
            }
        }
    }
}
