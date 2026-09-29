#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <type_traits>
#include <variant>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

struct Pixel {
    int x;
    int y;

    [[nodiscard]] constexpr auto operator==(const Pixel& other) const noexcept -> bool {
        return x == other.x && y == other.y;
    }
};

} // namespace

TEST_CASE("not_equal negates general value equality over the supported domain", "[not_equal][core]") {
    using cljonic::equal;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::not_equal;
    using cljonic::Queue;
    using cljonic::Range;
    using cljonic::Repeat;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.NotEqualFunction");
    TRACE_ID("invariant.NotEqualFunction.CanonicalNamedGeneralInequalityOperation");
    TRACE_ID("invariant.NotEqualFunction.NamedOperationDistinctFromOperatorNotEquals");
    TRACE_ID("invariant.NotEqualFunction.NegatesEqualOverSameSupportedDomain");
    TRACE_ID("invariant.NotEqualFunction.ThreeClojureNotEqualArities");
    TRACE_ID("invariant.NotEqualFunction.UnaryFormReturnsFalseForSingleSupportedDomainOperand");
    TRACE_ID("invariant.NotEqualFunction.BinaryFormNegatesEqual");
    TRACE_ID("invariant.NotEqualFunction.VariadicFormNegatesAdjacentPairConjunction");
    TRACE_ID("invariant.NotEqualFunction.AdjacentPairsAndUnaryOperandCompileTimeGated");
    TRACE_ID("invariant.NotEqualFunction.DomainGatingIdenticalToEqual");
    TRACE_ID("invariant.NotEqualFunction.TerminationIdenticalToEqual");
    TRACE_ID("invariant.NotEqualFunction.ConstexprNoexceptNonmutatingNonallocating");

    // ------------------------------------------------------------------------
    // Scalar fallthrough within the closed value domain, and the definitional
    // relationship not_equal(a, b) == !equal(a, b).
    // ------------------------------------------------------------------------
    STATIC_REQUIRE(not_equal(1, 2));
    STATIC_REQUIRE(!not_equal(1, 1));
    STATIC_REQUIRE(not_equal('a', 'b'));
    STATIC_REQUIRE(!not_equal('a', 'a'));
    enum class Color { Red, Green };
    STATIC_REQUIRE(not_equal(Color::Red, Color::Green));
    STATIC_REQUIRE(!not_equal(Color::Red, Color::Red));
    STATIC_REQUIRE(not_equal(Pixel{1, 2}, Pixel{1, 3}));
    STATIC_REQUIRE(!not_equal(Pixel{1, 2}, Pixel{1, 2}));
    STATIC_REQUIRE(not_equal(std::variant<int, long>{1}, std::variant<int, long>{2}));
    STATIC_REQUIRE(!not_equal(std::variant<int, long>{1}, std::variant<int, long>{1}));
    STATIC_REQUIRE(not_equal(1, 2) == !equal(1, 2));
    STATIC_REQUIRE(not_equal(1, 1) == !equal(1, 1));

    // ------------------------------------------------------------------------
    // Sequential family: mutual comparability by produced sequence, negated.
    // ------------------------------------------------------------------------
    constexpr auto vector_a = Vector<int, 4>{1, 2, 3};
    constexpr auto vector_b = Vector<int, 4>{1, 2, 3};
    STATIC_REQUIRE(not_equal(vector_a, vector_b) == !equal(vector_a, vector_b));
    STATIC_REQUIRE(not_equal(Vector<int, 4>{1, 2, 3}, Vector<int, 4>{1, 2, 4}));
    STATIC_REQUIRE(!not_equal(Vector<int, 4>{1, 2, 3}, Range<int>{1, 4, 1}));
    STATIC_REQUIRE(not_equal(Vector<int, 4>{1, 2, 3}, Queue<int, 4>{1, 2, 4}));
    STATIC_REQUIRE(!not_equal(Vector<int, 4>{1, 1, 1}, Repeat<int>{1, 3U}));
    // not_equal remains available for pairs with no native cross-type operator,
    // such as a Vector compared with a Range.
    STATIC_REQUIRE(not_equal(Vector<int, 4>{1, 2, 3}, Range<int>{1, 3, 1}));

    // ------------------------------------------------------------------------
    // Map, Set, String families: same-kind only, order-insensitive, negated.
    // ------------------------------------------------------------------------
    constexpr auto map_small = Map<int, int, 4>{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
    constexpr auto map_large = Map<int, int, 8>{MapEntry<int, int>{2, 20}, MapEntry<int, int>{1, 10}};
    STATIC_REQUIRE(!not_equal(map_small, map_large));
    STATIC_REQUIRE(not_equal(map_small, Map<int, int, 4>{MapEntry<int, int>{1, 11}}));
    STATIC_REQUIRE(not_equal(map_small, Map<int, int, 4>{MapEntry<int, int>{1, 10}}));
    constexpr auto set_small = Set<int, 4>{3, 1, 2};
    constexpr auto set_large = Set<int, 8>{1, 2, 3};
    STATIC_REQUIRE(!not_equal(set_small, set_large));
    STATIC_REQUIRE(not_equal(set_small, Set<int, 4>{1, 2, 4}));
    STATIC_REQUIRE(!not_equal(String<8>{"abc"}, String<16>{"abc"}));
    STATIC_REQUIRE(not_equal(String<8>{"abc"}, String<8>{"abd"}));

    // ------------------------------------------------------------------------
    // Arities: unary returns false for any admitted operand; the variadic form
    // negates the adjacent-pair conjunction.
    // ------------------------------------------------------------------------
    STATIC_REQUIRE(!not_equal(1));
    STATIC_REQUIRE(!not_equal(Color::Red));
    STATIC_REQUIRE(!not_equal(Pixel{1, 2}));
    STATIC_REQUIRE(!not_equal(vector_a));
    STATIC_REQUIRE(!not_equal(map_small));
    STATIC_REQUIRE(!not_equal(set_small));
    STATIC_REQUIRE(!not_equal(String<8>{"abc"}));
    STATIC_REQUIRE(!not_equal(Repeat<int>{7}));

    STATIC_REQUIRE(!not_equal(1, 1, 1));
    STATIC_REQUIRE(not_equal(1, 2, 1));
    STATIC_REQUIRE(not_equal(1, 1, 2));
    STATIC_REQUIRE(not_equal(1, 2, 2));
    STATIC_REQUIRE(!not_equal(1, 1, 1, 1, 1));
    STATIC_REQUIRE(not_equal(Vector<int, 4>{1, 2, 3}, Range<int>{1, 4, 1}, Vector<int, 4>{1, 2, 4}));
    STATIC_REQUIRE(!not_equal(Vector<int, 4>{1, 2, 3}, Range<int>{1, 4, 1}, Vector<int, 4>{1, 2, 3}));

    // ------------------------------------------------------------------------
    // Termination is identical to equal: unbounded pairs compare the configured
    // observable traversal cap, so every call terminates.
    // ------------------------------------------------------------------------
    STATIC_REQUIRE(!not_equal(Repeat<int>{7}, Repeat<int>{7}));
    STATIC_REQUIRE(not_equal(Repeat<int>{7}, Repeat<int>{8}));
    STATIC_REQUIRE(!not_equal(Range<int>{0, 5, 0}, Range<int>{0, 9, 0}));

    // ------------------------------------------------------------------------
    // Domain gating is identical to equal: the same concept-level admission.
    // The compile-fail harness (scripts/check-not-equal-compile-failures.py)
    // proves that unsupported, mixed, cross-family, and floating-point operands
    // fail to compile in every arity, matching equal.
    // ------------------------------------------------------------------------
    STATIC_REQUIRE_FALSE(cljonic::concepts::StableEqualityComparable<double>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::StableEqualityComparable<Vector<float, 2>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::StableEqualityComparable<std::variant<int, double>>);

    // ------------------------------------------------------------------------
    // constexpr, noexcept, non-mutating, non-allocating.
    // ------------------------------------------------------------------------
    STATIC_REQUIRE(noexcept(not_equal(1, 1)));
    STATIC_REQUIRE(noexcept(not_equal(1)));
    STATIC_REQUIRE(noexcept(not_equal(1, 1, 1)));

    // ------------------------------------------------------------------------
    // Runtime coverage: volatile-derived values defeat constexpr folding so
    // every overload and branch genuinely executes (coverage gate).
    // ------------------------------------------------------------------------
    volatile int runtime_raw = 1;
    const auto runtime_one = runtime_raw;
    volatile int runtime_other_raw = 2;
    const auto runtime_two = runtime_other_raw;

    CHECK_FALSE(not_equal(runtime_one));
    CHECK(not_equal(runtime_one, runtime_two));
    CHECK_FALSE(not_equal(runtime_one, runtime_one));
    CHECK(not_equal(runtime_one, runtime_two, runtime_one));
    CHECK_FALSE(not_equal(runtime_one, runtime_one, runtime_one));

    auto runtime_vector = Vector<int, 4>{runtime_one, runtime_two};
    auto runtime_vector_diff = Vector<int, 4>{runtime_one, runtime_one};
    CHECK(not_equal(runtime_vector, runtime_vector_diff));
    CHECK_FALSE(not_equal(runtime_vector, runtime_vector));
    CHECK_FALSE(not_equal(runtime_vector, Range<int>{runtime_one, 3, 1})); // produces [1, 2]
    CHECK(not_equal(runtime_vector, Range<int>{runtime_one, 2, 1}));       // produces [1]

    auto runtime_queue = Queue<int, 4>{runtime_one, runtime_two};
    CHECK_FALSE(not_equal(runtime_vector, runtime_queue));

    auto runtime_map = Map<int, int, 4>{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
    auto runtime_map_reordered = Map<int, int, 8>{MapEntry<int, int>{2, 20}, MapEntry<int, int>{1, 10}};
    auto runtime_map_small = Map<int, int, 4>{MapEntry<int, int>{1, 10}};
    auto runtime_map_value = Map<int, int, 4>{MapEntry<int, int>{1, 11}, MapEntry<int, int>{2, 20}};
    CHECK_FALSE(not_equal(runtime_map, runtime_map_reordered));
    CHECK(not_equal(runtime_map, runtime_map_small));
    CHECK(not_equal(runtime_map, runtime_map_value));

    auto runtime_set = Set<int, 4>{3, 1, 2};
    auto runtime_set_reordered = Set<int, 8>{1, 2, 3};
    auto runtime_set_member = Set<int, 4>{1, 2, 4};
    CHECK_FALSE(not_equal(runtime_set, runtime_set_reordered));
    CHECK(not_equal(runtime_set, runtime_set_member));

    auto runtime_string = String<8>{"abc"};
    auto runtime_string_same = String<16>{"abc"};
    auto runtime_string_diff = String<8>{"abd"};
    CHECK_FALSE(not_equal(runtime_string, runtime_string_same));
    CHECK(not_equal(runtime_string, runtime_string_diff));

    auto runtime_repeat = Repeat<int>{runtime_one};
    CHECK_FALSE(not_equal(runtime_repeat, runtime_repeat));
}
