/**
 * Test the thread-safe MessageBuffer.
 */

#include <thread>
#include <chrono>
#include <optional>
#include <vector>

#include <catch2/catch.hpp>

#include <src/cpp/lennardjonesium/tools/message_buffer.hpp>

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

    WHEN("I send the close() signal")
    {
        message_buffer.put(1);
        message_buffer.put(2);
        message_buffer.close();

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
        message_buffer.close();
        consumer.join();

        THEN("The output vector now has the same contents as the input")
        {
            REQUIRE(input == output);
        }
    }
}

SCENARIO("Message buffer with multiple producers and consumers")
{
    tools::MessageBuffer<int> message_buffer;

    std::vector<int> even = {0, 2, 4, 6, 8};
    std::vector<int> odd = {1, 3, 5, 7, 9};

    std::vector<int> out1;
    std::vector<int> out2;

    WHEN("I create producer and consumer threads and join them")
    {
        std::thread producer1(
            [&message_buffer, &even]()
            {
                for (auto i : even)
                {
                    message_buffer.put(i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        );
        
        std::thread producer2(
            [&message_buffer, &odd]()
            {
                for (auto i : odd)
                {
                    message_buffer.put(i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        );

        std::thread consumer1(
            [&message_buffer, &out1]()
            {
                while (auto o = message_buffer.get())
                {
                    out1.push_back(o.value());
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        );

        std::thread consumer2(
            [&message_buffer, &out2]()
            {
                while (auto o = message_buffer.get())
                {
                    out2.push_back(o.value());
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        );

        producer1.join();
        producer2.join();
        message_buffer.close();
        consumer1.join();
        consumer2.join();

        THEN("The output vector now has the same contents as the input")
        {
            std::vector input = even;
            input.insert(input.end(), odd.begin(), odd.end());

            std::vector output = out1;
            output.insert(output.end(), out2.begin(), out2.end());

            REQUIRE_THAT(output, Catch::UnorderedEquals(input));
        }
    }
}
