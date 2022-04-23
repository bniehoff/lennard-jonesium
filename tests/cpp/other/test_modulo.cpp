/**
 * Making sure I understand which way the modulo operator works with negative operands
 */

#include <cstdlib>

#include <catch2/catch.hpp>

SCENARIO( "Modulus operator with negative operands" ) {
    std::div_t result;

    WHEN( "I divide a negative integer by a larger positive divisor" ) {
        result = std::div(-1, 5);

        THEN( "The quotient is zero" ) {
            REQUIRE( result.quot == 0 );
        }

        THEN( "The remainder is negative" ) {
            REQUIRE( result.rem == -1 );
        }
    }

    WHEN( "I divide a positive integer by a larger positive divisor" ) {
        result = std::div(1, 5);

        THEN( "The quotient is zero" ) {
            REQUIRE( result.quot == 0 );
        }

        THEN( "The remainder is positive" ) {
            REQUIRE( result.rem == 1 );
        }
    }

    WHEN( "I divide a positive integer by a smaller positive divisor" ) {
        result = std::div(6, 5);

        THEN( "The quotient is positive" ) {
            REQUIRE( result.quot == 1 );
        }

        THEN( "The remainder is positive" ) {
            REQUIRE( result.rem == 1 );
        }
    }
}
