/**
 * A simple test of the friendly greeter system
 */

#include <catch2/catch.hpp>

#include <lennardjonesium/greeter/greeter.hpp>

#include <string>

SCENARIO( "Interaction with the greeter" ) {
    WHEN( "I give the greeter my name" ) {
        std::string my_name{"Ben"};

        THEN( "The greeter gives me a personalized greeting" ) {
            std::string greeting = greet(my_name);

            REQUIRE( "Hello, Ben, and welcome to C++!" == greeting);
        }
    }
}
