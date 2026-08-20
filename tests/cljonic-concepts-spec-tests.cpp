#include <catch2/catch_test_macros.hpp>

#include <cljonic-concepts.hpp>

TEST_CASE("Collection requires an exact size_t result", "[concepts]") {
  struct ExactSize {
    constexpr auto size() const noexcept -> std::size_t { return 3U; }
  } exact_size;

  struct ConvertibleSize {
    constexpr auto size() const noexcept -> unsigned int { return 3U; }
  } convertible_size;

  STATIC_REQUIRE(cljonic::concepts::Collection<decltype(exact_size)>);
  STATIC_REQUIRE_FALSE(
      cljonic::concepts::Collection<decltype(convertible_size)>);
}
