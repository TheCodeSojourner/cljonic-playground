#include <catch2/catch_test_macros.hpp>

#include <cljonic-concepts.hpp>
#include <cljonic-vector.hpp>

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

TEST_CASE("Vector element storage requires non-throwing operations",
          "[vector][concepts]") {
  struct ThrowingDefault {
    ThrowingDefault() noexcept(false) {}
    ThrowingDefault(const ThrowingDefault &) noexcept = default;
    ThrowingDefault &operator=(const ThrowingDefault &) noexcept = default;
  };

  struct ThrowingAssignment {
    ThrowingAssignment() noexcept = default;
    ThrowingAssignment(const ThrowingAssignment &) noexcept = default;
    ThrowingAssignment &operator=(const ThrowingAssignment &) noexcept(false) {
      return *this;
    }
  };

  STATIC_REQUIRE(cljonic::concepts::VectorElement<ThrowingDefault>);
  STATIC_REQUIRE(cljonic::concepts::VectorElement<ThrowingAssignment>);
  STATIC_REQUIRE_FALSE(
      cljonic::concepts::NothrowVectorElement<ThrowingDefault>);
  STATIC_REQUIRE_FALSE(
      cljonic::concepts::NothrowVectorElement<ThrowingAssignment>);
  STATIC_REQUIRE(cljonic::concepts::NothrowVectorElement<int>);
  STATIC_REQUIRE(noexcept(cljonic::Vector<int, 4>{1, 2}));
}

TEST_CASE("Element construction requires non-throwing value copies",
          "[concepts]") {
  struct ThrowingCopy {
    ThrowingCopy() noexcept = default;
    ThrowingCopy(const ThrowingCopy &) noexcept(false) {}
    operator int() const noexcept { return 1; }
  } argument;

  STATIC_REQUIRE_FALSE(
      cljonic::concepts::NothrowElementConstruction<int, decltype(argument)>);
}
