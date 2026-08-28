#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

TEST_CASE("Vector element storage requires non-throwing operations", "[vector][concepts]") {
    struct ThrowingDefault {
        ThrowingDefault() noexcept(false) {
        }
        ThrowingDefault(const ThrowingDefault&) noexcept = default;
        ThrowingDefault& operator=(const ThrowingDefault&) noexcept = default;
    };

    struct ThrowingAssignment {
        ThrowingAssignment() noexcept = default;
        ThrowingAssignment(const ThrowingAssignment&) noexcept = default;
        ThrowingAssignment& operator=(const ThrowingAssignment&) noexcept(false) {
            return *this;
        }
    };

    STATIC_REQUIRE(cljonic::concepts::VectorElement<ThrowingDefault>);
    STATIC_REQUIRE(cljonic::concepts::VectorElement<ThrowingAssignment>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowVectorElement<ThrowingDefault>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowVectorElement<ThrowingAssignment>);
    STATIC_REQUIRE(cljonic::concepts::NothrowVectorElement<int>);
    STATIC_REQUIRE(noexcept(cljonic::Vector<int, 4>{1, 2}));
}

TEST_CASE("Element construction requires non-throwing value copies", "[concepts]") {
    struct ThrowingCopy {
        ThrowingCopy() noexcept = default;
        ThrowingCopy(const ThrowingCopy&) noexcept(false) {
        }
        operator int() const noexcept {
            return 1;
        }
    } argument;

    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowElementConstruction<int, decltype(argument)>);
}
