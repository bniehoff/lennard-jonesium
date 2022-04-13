/**
 * Making sure I understand how to keep a reference to a unique pointer even after the unique
 * pointer is moved.
 */

#include <utility>
#include <memory>

#include <catch2/catch.hpp>

struct Data
{
    int value;
};

struct Owner
{
    std::unique_ptr<Data> data;
};

struct Modifier
{
    Data& data;

    void modify(int amount)
    {
        data.value += amount;
    }
};

SCENARIO("Referencing a moved unique pointer")
{
    int initial_value = 2;
    auto data = std::make_unique<Data>(initial_value);

    Modifier m(*data);

    Owner o(std::move(data));

    REQUIRE(nullptr == data);

    WHEN("I modify the data via the Modifier")
    {
        int modify_amount = 7;

        m.modify(modify_amount);

        THEN("The data held by the Owner changes")
        {
            REQUIRE((initial_value + modify_amount) == o.data->value);
        }
    }
}
