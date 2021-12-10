/**
 * Testing CellListArray object
 */

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/dimensions.hpp>
#include <src/lennardjonesium/tools/cell_list_array.hpp>

using Eigen::Vector4i;

SCENARIO( "Creating a CellListArray with unit dimensions" ) {
    // We use a 3x3x3 box
    tools::Dimensions dimensions{1.0};

    WHEN( "I create a CellListArray with 0.3 cutoff" ) {
        double cutoff_length{0.3};

        tools::CellListArray cell_list_array{dimensions, cutoff_length};

        THEN( "It has 3x3x3 elements" ) {
            // TODO: Need to understand how to get size of boost::multi_array
        }

        THEN( "I can access the elements as expected" ) {
            tools::CellList entry{1, 3, 4, 6, 7, 9};

            cell_list_array(2, 1, 1) = entry;

            REQUIRE( entry == cell_list_array(2, 1, 1) );
        }
    }
}
