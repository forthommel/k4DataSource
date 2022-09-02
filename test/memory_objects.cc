#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "k4DataSource/k4Record.h"

TEST_CASE("k4DataSource", "[memory]") {
  SECTION("simple objects") {
    int test = 42, result;
    k4Record ptr(&test);
    ptr.fill(result);
    REQUIRE(result == test);

    result = *ptr.getAs<int>();
    REQUIRE(result == test);
  }
  SECTION("collections") {
    std::vector<double> test = {M_PI, 2.5}, result;
    k4Record ptr(&test);
    ptr.fill(result);
    REQUIRE(result == test);

    auto* ptr_result = ptr.getAs<std::vector<double> >();
    REQUIRE(*ptr_result == test);

    test[1] = 4.2;
    REQUIRE(ptr_result->at(1) == test[1]);
  }
  SECTION("complex collections") {
    struct Foo {
      bool operator==(const Foo& oth) const { return bar == oth.bar && baz == oth.baz; }
      bool bar{false};
      int baz{42};
    };
    std::vector<Foo> foos(2), result;
    k4Record ptr(&foos);
    ptr.fill(result);
    REQUIRE(result.size() == foos.size());
    REQUIRE(result == foos);

    auto* ptr_result = ptr.getAs<std::vector<Foo> >();
    REQUIRE(*ptr_result == foos);

    foos[1].baz = 21;
    REQUIRE(result[1].baz == 42);  // check that filling does not make the copy dependent on mother object
    REQUIRE(ptr_result->at(1).baz == foos[1].baz);
  }
}
