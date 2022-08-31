#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "k4DataSource/k4Record.h"

TEST_CASE("k4DataSource", "[memory]") {
  SECTION("simple objects") {
    int test = 42, result;
    k4Handle ptr(&test);
    ptr >> result;
    REQUIRE(result == test);

    result = *ptr.getAs<int>();
    REQUIRE(result == test);
  }
  SECTION("collections") {
    std::vector<double> test = {M_PI, 2.5}, result;
    k4Handle ptr(&test);
    ptr >> result;
    REQUIRE(result == test);

    auto* ptr_result = ptr.getAs<std::vector<double> >();
    REQUIRE(*ptr_result == test);
  }
}
