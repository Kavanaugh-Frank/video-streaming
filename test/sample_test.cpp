#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Basic arithmetic works", "[math]") {
    REQUIRE(1 + 1 == 2);
    REQUIRE(2 * 2 == 4);
    REQUIRE(4 / 2 == 2);
}

TEST_CASE("String concatenation", "[string]") {
    std::string a = "Hello";
    std::string b = "World";
    REQUIRE((a + " " + b) == "Hello World");
}
