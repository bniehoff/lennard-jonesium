/**
 * Test the thread-safe MessageBuffer.
 */

#include <thread>
#include <chrono>
#include <optional>
#include <vector>

#include <catch2/catch.hpp>
#include <Eigen/Dense>

#include <src/lennardjonesium/tools/message_buffer.hpp>

SCENARIO("Message buffer in single-threaded environment")
{
    tools::MessageBuffer<int> message_buffer;

    WHEN("I push some values to the buffer")
    {
        message_buffer.put(1);
        message_buffer.put(2);
        message_buffer.put(3);

        THEN("I read back the same values")
        {
            REQUIRE(1 == message_buffer.get());
            REQUIRE(2 == message_buffer.get());
            REQUIRE(3 == message_buffer.get());
        }
    }

    WHEN("I send the end() signal")
    {
        message_buffer.put(1);
        message_buffer.put(2);
        message_buffer.end();

        THEN("I read back std::nullopt")
        {
            REQUIRE(1 == message_buffer.get());
            REQUIRE(2 == message_buffer.get());
            REQUIRE(std::nullopt == message_buffer.get());
        }
    }
}

SCENARIO("Message buffer with producer and consumer threads")
{
    tools::MessageBuffer<int> message_buffer;

    std::vector<int> input = {1, 3, 5, 7, 9, 2, 4, 6, 8, 0};
    std::vector<int> output;

    WHEN("I create producer and consumer threads and join them")
    {
        std::thread producer(
            [&message_buffer, &input]()
            {
                for (auto i : input)
                {
                    message_buffer.put(i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }

                message_buffer.end();
            }
        );

        std::thread consumer(
            [&message_buffer, &output]()
            {
                while (auto o = message_buffer.get())
                {
                    output.push_back(o.value());
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        );

        producer.join();
        consumer.join();

        THEN("The output vector now has the same contents as the input")
        {
            REQUIRE(input == output);
        }
    }
}
