/**
 * A simple greeter function, but this time in C++
 */

#include <string>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <lennardjonesium/greeter/greeter.hpp>

void Greeter::greet(std::string name)
{
    fmt::print(
        // output_stream_,
        "Hello, {}, and welcome to C++!  My favorite numbr is {}\n",
        name,
        favorite_number_
    );

    // output_stream_.flush();
}
