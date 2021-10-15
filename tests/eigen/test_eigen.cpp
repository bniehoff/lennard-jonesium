/**
 * This "test" is just for me to understand how to do things with Eigen
 */

#include <vector>
#include <ranges>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

using Eigen::MatrixXi;
using Eigen::Vector4i;

SCENARIO( "Views of a Matrix" ) {
    MatrixXi positions{
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
        {1, 1, 0, 1, 0, 0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> cells{{2, 4, 5, 9}, {0, 1, 3, 6, 7, 8}};

    WHEN( "I modify one of the views" ) {
        Vector4i v{1, -1, 0, 0};
        int cell_index{0};

        positions(Eigen::all, cells[cell_index]).colwise() += v;

        THEN( "The original matrix changes" ) {
            for (auto index : cells[cell_index]) {
                REQUIRE( index + 1  == positions(0, index) );
                REQUIRE( 8 - index  == positions(1, index) );
                REQUIRE( cell_index == positions(2, index) );
            }
        }
    }

    WHEN( "I sum the values in one of the viws" ) {
        int cell_index{0};

        positions(3, cells[cell_index]) = positions({0, 1}, cells[cell_index]).colwise().sum();

        THEN( "We get the result in the last row" ) {
            for (auto index : cells[cell_index]) {
                REQUIRE( index      == positions(0, index) );
                REQUIRE( 9 - index  == positions(1, index) );
                REQUIRE( cell_index == positions(2, index) );
                REQUIRE( 9          == positions(3, index) );
            }
        }
    }
}
