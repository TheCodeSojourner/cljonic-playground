#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Contains free function operates on each collection lookup domain", "[contains]") {
    using cljonic::assoc;
    using cljonic::conj;
    using cljonic::contains;
    using cljonic::Map;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Contains");
    TRACE_ID("invariant.Contains.ContainsFreeFunctionSupported");
    TRACE_ID("invariant.Contains.ReturnsBooleanMembership");
    TRACE_ID("invariant.Contains.TestsCollectionLookupDomain");
    TRACE_ID("invariant.Contains.NoHeapAllocation");
    TRACE_ID("invariant.Contains.NoRtti");
    TRACE_ID("invariant.Contains.NoExceptions");
    TRACE_ID("invariant.Contains.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Contains.ReferentialTransparency");
    TRACE_ID("invariant.Contains.ContainsDoesNotPerformDefaultReturningAccess");
    TRACE_ID("invariant.Map.ContainsIsCanonicalKeyPresencePredicate");
    TRACE_ID("invariant.Set.ContainsIsCanonicalMembershipPredicate");
    TRACE_ID("invariant.Vector.ContainsIsCanonicalIndexPredicate");
    TRACE_ID("invariant.String.ContainsIsCanonicalIndexPredicate");

    // Map: key presence.
    constexpr auto m = assoc(Map<int, int, 4>{}, 1, 100);
    STATIC_REQUIRE(contains(m, 1));
    STATIC_REQUIRE_FALSE(contains(m, 2));

    // Set: element presence.
    constexpr auto s = conj(Set<int, 4>{}, 5);
    STATIC_REQUIRE(contains(s, 5));
    STATIC_REQUIRE_FALSE(contains(s, 8));

    // Vector: index in range.
    constexpr Vector<int, 4> v{10, 20, 30};
    STATIC_REQUIRE(contains(v, 0U));
    STATIC_REQUIRE(contains(v, 2U));
    STATIC_REQUIRE_FALSE(contains(v, 3U));

    // String: index in range.
    constexpr String<8> st{"abc"};
    STATIC_REQUIRE(contains(st, 0U));
    STATIC_REQUIRE(contains(st, 2U));
    STATIC_REQUIRE_FALSE(contains(st, 3U));

    // Runtime forms.
    const auto m_runtime = assoc(Map<int, int, 4>{}, 7, 70);
    CHECK(contains(m_runtime, 7));
    CHECK_FALSE(contains(m_runtime, 8));

    const auto s_runtime = conj(Set<int, 4>{}, 9);
    CHECK(contains(s_runtime, 9));
    CHECK_FALSE(contains(s_runtime, 10));

    const auto v_runtime = Vector<int, 4>{1, 2};
    CHECK(contains(v_runtime, 1U));
    CHECK_FALSE(contains(v_runtime, 2U));

    const auto st_runtime = String<8>{"xy"};
    CHECK(contains(st_runtime, 1U));
    CHECK_FALSE(contains(st_runtime, 2U));
}