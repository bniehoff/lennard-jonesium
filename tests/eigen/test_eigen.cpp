/**
 * This "test" is just for me to understand how to do things with Eigen
 */

#include <vector>
#include <ranges>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

// using Eigen::MatrixXi;
using Eigen::Vector4i;
using Eigen::Vector3i;
using Eigen::Matrix4Xi;

SCENARIO( "Views of a Matrix" ) {
    Matrix4Xi positions{
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
        {1, 1, 0, 1, 0, 0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::vector<std::vector<int>> cells{{2, 4, 5, 9}, {0, 1, 3, 6, 7, 8}};

    WHEN( "I modify one of the views" ) {
        Vector4i v{1, -1, 0, 0};
        int cell_index{0};

        auto view = positions(Eigen::all, cells[cell_index]).colwise();
        
        view += v;

        THEN( "The original matrix changes" ) {
            for (auto index : cells[cell_index]) {
                REQUIRE( index + 1  == positions(0, index) );
                REQUIRE( 8 - index  == positions(1, index) );
                REQUIRE( cell_index == positions(2, index) );
            }
        }
    }

    WHEN( "I sum the values in one of the views" ) {
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

SCENARIO( "Components of a 4-vector" ) {
    Vector4i v{1, 2, 3, 4};

    WHEN( "I access the components of a 4-vector by name" ) {
        THEN( "I get the expected values" ) {
            REQUIRE( 1 == v.x() );
            REQUIRE( 2 == v.y() );
            REQUIRE( 3 == v.z() );
            REQUIRE( 4 == v.w() );
        }
    }
}

SCENARIO( "Cross products" ) {
    WHEN( "I take the cross product between 3-vectors" ) {
        Vector3i a = Vector3i::UnitX();
        Vector3i b = Vector3i::UnitY();

        Vector3i c = a.cross(b);

        THEN( "I get the expected result" ) {
            REQUIRE( Vector3i::UnitZ() == c );
        }
    }

    WHEN( "I take the cross product between 3-vector blocks of 4-vectors" ) {
        Vector4i a = Vector4i::UnitX();
        Vector4i b = Vector4i::UnitY();;

        Vector4i c = a.cross3(b);

        THEN( "I get the expected result" ) {
            REQUIRE( Vector4i::UnitZ() == c );
        }
    }

    WHEN( "I define my vectors as columns of a matrix" ) {
        Matrix4Xi abc{
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 0},
            {0, 0, 0}
        };

        abc.col(2) = abc.col(0).cross3(abc.col(1));

        THEN( "I get the expected result" ) {
            REQUIRE( Vector4i::UnitZ() == abc.col(2) );
        }
    }
}
