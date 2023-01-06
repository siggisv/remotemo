#include <catch2/catch_test_macros.hpp>

#include <cstdint>

// Dummy test that includes mocking just to see if everything
// is correctly setup for using Catch2

uint32_t factorial(uint32_t number)
{
  return number <= 1 ? number : factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]")
{
  REQUIRE(factorial(1) == 1);
  REQUIRE(factorial(2) == 2);
  REQUIRE(factorial(3) == 6);
  REQUIRE(factorial(10) == 3'628'800);
}
