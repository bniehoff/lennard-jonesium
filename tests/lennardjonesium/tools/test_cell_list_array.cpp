/**
 * Testing CellListArray object
 */

#include <algorithm>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/dimensions.hpp>
#include <src/lennardjonesium/tools/cell_list_array.hpp>

SCENARIO( "Creating a CellListArray and accessing elements" ) {
    tools::Dimensions dimensions{1.0};

    WHEN( "I create a CellListArray with 0.3 cutoff" ) {
        double cutoff_length{0.3};

        tools::CellListArray cell_list_array{dimensions, cutoff_length};

        THEN( "It has 3x3x3 elements" ) {
            REQUIRE( 3 == cell_list_array.shape()[0] );
            REQUIRE( 3 == cell_list_array.shape()[1] );
            REQUIRE( 3 == cell_list_array.shape()[2] );
        }

        THEN( "I can access the elements as expected" ) {
            tools::CellList entry{1, 3, 4, 6, 7, 9};

            cell_list_array(2, 1, 1) = entry;

            REQUIRE( entry == cell_list_array(2, 1, 1) );
        }
    }
}

SCENARIO( "Using the cell_view() generator on a 3x3x3 array" ) {
    tools::Dimensions dimensions{1.0};
    double cutoff_length{0.3};
    tools::CellListArray cell_list_array{dimensions, cutoff_length};

    WHEN( "I iterate over the cells in a 3x3x3 array" ) {
        int count{0};

        for (auto& cell_list : cell_list_array.cell_view()) {
            count++;
        }

        THEN( "I get the correct number of cells" ) {
            REQUIRE( 27 == count );
        }
    }

    WHEN( "I record the order in which cells are visited" ) {
        int index{0};

        for (auto& cell_list : cell_list_array.cell_view()) {
            cell_list.push_back(index);
            index++;
        }

        THEN( "I get the expected order of access" ) {
            for (int i = 0; i < cell_list_array.shape()[0]; i++)
                for (int j = 0; j < cell_list_array.shape()[1]; j++)
                    for (int k = 0; k < cell_list_array.shape()[2]; k++) {
                        index = 9*i + 3*j + k;
                        REQUIRE( index == cell_list_array(i, j, k)[0] );
                    }
        }
    }
}

void setup_cell_list_array(tools::CellListArray& cell_list_array)
{
    // To set up, let's populate the cell list array with arrays of indices, so that we can
    // easily see which cells have been selected
    for (int i = 0; i < cell_list_array.shape()[0]; i++)
        for (int j = 0; j < cell_list_array.shape()[1]; j++)
            for (int k = 0; k < cell_list_array.shape()[2]; k++)
                cell_list_array(i, j, k) = tools::CellList{i, j, k};
}

SCENARIO( "Getting a neighbor pair from index and displacement" ) {
    // To access a protected method for testing, we define a derived class which makes it public
    class TestCellListArray : public tools::CellListArray
    {
        public:
            using tools::CellListArray::get_neighbor_pair_;
            using tools::CellListArray::multi_index_type;
            using tools::CellListArray::CellListArray;
    };

    // Now create a 3x3x3 cell list array so that we can test different corner cases
    tools::Dimensions dimensions{1.0};
    double cutoff_length{0.3};
    TestCellListArray cell_list_array{dimensions, cutoff_length};

    setup_cell_list_array(cell_list_array);
    
    WHEN( "I try a displacement that does not wrap around" ) {
        auto neighbor_pair = cell_list_array.get_neighbor_pair_(
            TestCellListArray::multi_index_type {1, 1, 1},
            TestCellListArray::multi_index_type {0, 0, 1}
        );

        THEN( "I get the correct pair of cells, with zero offset" ) {
            REQUIRE( tools::CellList {1, 1, 1}    == neighbor_pair.first  );
            REQUIRE( tools::CellList {1, 1, 2}    == neighbor_pair.second );
            REQUIRE( Eigen::Vector4i {0, 0, 0, 0} == neighbor_pair.offset );
        }
    }

    WHEN( "I try a displacement that wraps around" ) {
        auto neighbor_pair = cell_list_array.get_neighbor_pair_(
            TestCellListArray::multi_index_type {0, 1, 2},
            TestCellListArray::multi_index_type {-1, 1, 1}
        );

        THEN( "I get the correct pair of cells, with correct offset" ) {
            REQUIRE( tools::CellList {0, 1, 2}     == neighbor_pair.first  );
            REQUIRE( tools::CellList {2, 2, 0}     == neighbor_pair.second );
            REQUIRE( Eigen::Vector4i {-1, 0, 1, 0} == neighbor_pair.offset );
        }
    }
}

// This function will be useful in the following tests:
struct NeighborCodes {int first; int second;};

NeighborCodes get_neighbor_codes(tools::NeighborPair np, tools::CellListArray cla)
{
    // We assume that the cell lists have been set up to store their own indices using
    // setup_cell_list_array() above
    int displacement[3];
    
    for (int d = 0; d < 3; d++)
        displacement[d] = np.second[d] - np.first[d] + np.offset[d] * cla.shape()[d];
    
    // Each possible displacement is then mapped to a number from 0 to 26.
    // {0, 0, 0} is mapped to 13.
    return NeighborCodes {
        9 * (1 + displacement[0]) + 3 * (1 + displacement[1]) + (1 + displacement[2]),
        9 * (1 - displacement[0]) + 3 * (1 - displacement[1]) + (1 - displacement[2])
    };
}

SCENARIO( "Using the neighbor_view() generator various-sized arrays" ) {
    // The list of neighbor codes should contain every entry from 0 to 26, except 13
    const tools::CellList all_neighbor_codes {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26
    };

    GIVEN( "A 3x3x3 array set up with its own indices" ) {
        tools::Dimensions dimensions{1.0};
        double cutoff_length{0.3};

        tools::CellListArray cell_list_array{dimensions, cutoff_length};
        setup_cell_list_array(cell_list_array);

        // Separate result array, since the elements of a NeighborPair are const
        tools::CellListArray neighbor_codes_array(dimensions, cutoff_length);

        WHEN( "I append neighbor codes to all neighbors in pairs" ) {
            for (auto neighbor_pair : cell_list_array.neighbor_view())
            {
                auto neighbor_codes = get_neighbor_codes(neighbor_pair, cell_list_array);

                neighbor_codes_array(
                    neighbor_pair.first[0], neighbor_pair.first[1], neighbor_pair.first[2]
                ).push_back(neighbor_codes.first);

                neighbor_codes_array(
                    neighbor_pair.second[0], neighbor_pair.second[1], neighbor_pair.second[2]
                ).push_back(neighbor_codes.second);
            }

            THEN( "Every cell has interacted with each of its 26 neighbors exactly once" ) {
                for (auto& neighbor_code_list : neighbor_codes_array.cell_view())
                {
                    std::sort(neighbor_code_list.begin(), neighbor_code_list.end());
                    REQUIRE( all_neighbor_codes == neighbor_code_list );
                }
            }
        }
    }

    GIVEN( "A 2x2x2 array set up with its own indices" ) {
        tools::Dimensions dimensions{1.0};
        double cutoff_length{0.48};

        tools::CellListArray cell_list_array{dimensions, cutoff_length};
        setup_cell_list_array(cell_list_array);

        // Separate result array, since the elements of a NeighborPair are const
        tools::CellListArray neighbor_codes_array(dimensions, cutoff_length);

        WHEN( "I append neighbor codes to all neighbors in pairs" ) {
            for (auto neighbor_pair : cell_list_array.neighbor_view())
            {
                auto neighbor_codes = get_neighbor_codes(neighbor_pair, cell_list_array);

                neighbor_codes_array(
                    neighbor_pair.first[0], neighbor_pair.first[1], neighbor_pair.first[2]
                ).push_back(neighbor_codes.first);

                neighbor_codes_array(
                    neighbor_pair.second[0], neighbor_pair.second[1], neighbor_pair.second[2]
                ).push_back(neighbor_codes.second);
            }

            THEN( "Every cell has interacted with each of its 26 neighbors exactly once" ) {
                for (auto& neighbor_code_list : neighbor_codes_array.cell_view())
                {
                    std::sort(neighbor_code_list.begin(), neighbor_code_list.end());
                    REQUIRE( all_neighbor_codes == neighbor_code_list );
                }
            }
        }
    }

    GIVEN( "A 1x1x1 array set up with its own indices" ) {
        tools::Dimensions dimensions{1.0};
        double cutoff_length{0.67};

        tools::CellListArray cell_list_array{dimensions, cutoff_length};
        setup_cell_list_array(cell_list_array);

        // Separate result array, since the elements of a NeighborPair are const
        tools::CellListArray neighbor_codes_array(dimensions, cutoff_length);

        WHEN( "I append neighbor codes to all neighbors in pairs" ) {
            for (auto neighbor_pair : cell_list_array.neighbor_view())
            {
                auto neighbor_codes = get_neighbor_codes(neighbor_pair, cell_list_array);

                neighbor_codes_array(
                    neighbor_pair.first[0], neighbor_pair.first[1], neighbor_pair.first[2]
                ).push_back(neighbor_codes.first);

                neighbor_codes_array(
                    neighbor_pair.second[0], neighbor_pair.second[1], neighbor_pair.second[2]
                ).push_back(neighbor_codes.second);
            }

            THEN( "Every cell has interacted with each of its 26 neighbors exactly once" ) {
                for (auto& neighbor_code_list : neighbor_codes_array.cell_view())
                {
                    std::sort(neighbor_code_list.begin(), neighbor_code_list.end());
                    REQUIRE( all_neighbor_codes == neighbor_code_list );
                }
            }
        }
    }
}
