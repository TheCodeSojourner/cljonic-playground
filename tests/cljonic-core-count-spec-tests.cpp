#include <catch2/catch_test_macros.hpp>

#include <cljonic-core.hpp>

TEST_CASE("count is available for a type with logical size", "[count]") {
  struct Collection {
    constexpr auto size() const noexcept -> std::size_t { return 3U; }
  } collection;

  STATIC_REQUIRE(cljonic::count(collection) == 3U);
}
