/**
 * A simple greeter function, but this time in C++
 */

#ifndef LJ_GREETER_HPP
#define LJ_GREETER_HPP

#include <string>
#include <iostream>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>

using tee_device = boost::iostreams::tee_device<std::ostream, boost::iostreams::file_sink>;
using tee_stream = boost::iostreams::stream<tee_device>;

class Greeter
{
    public:
        void greet(std::string name);

        // Greeter() : Greeter(7) {}

        Greeter(int favorite_number) : favorite_number_{favorite_number} {}

        // Greeter(std::string filename)
        //     : output_stream_{std::cout, boost::iostreams::file_sink{filename}}
        // {}
    
    private:
        // tee_stream output_stream_;
        int favorite_number_;
};

#endif
