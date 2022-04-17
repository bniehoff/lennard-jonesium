/**
 * Making sure I understand how to use boost::iostreams::tee_device
 */

#include <filesystem>
#include <memory>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>

#include <catch2/catch.hpp>

SCENARIO("Writing content to file and console simultaneously")
{
    namespace fs = std::filesystem;
    namespace bio = boost::iostreams;

    fs::path test_dir{"test_tee_device"};
    fs::create_directory(test_dir);

    fs::path output_file = test_dir / "lorem_ipsum.txt";

    std::string lorem_ipsum =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor\n"
        "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud\n"
        "exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure\n"
        "dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n"
        "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt\n"
        "mollit anim id est laborum.\n";

    GIVEN("A tee device printing both to a file and to console")
    {
        using tee_device = bio::tee_device<std::ostream, bio::file_sink>;
        using tee_stream = bio::stream<tee_device>;

        auto file_sink = bio::file_sink(output_file);

        auto output_stream = tee_stream(tee_device(std::cout, file_sink));

        WHEN("I send text to the stream and close it")
        {
            output_stream << lorem_ipsum;

            output_stream.close();

            THEN("I read back the expected contents")
            {
                std::ifstream fin{output_file};
                std::ostringstream contents;

                contents << fin.rdbuf();

                REQUIRE(lorem_ipsum == contents.view());
            }
        }
    }

    // Clean up
    fs::remove_all(test_dir);
}