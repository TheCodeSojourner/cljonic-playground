#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

template <typename Collection, typename Index>
concept ContainsMemberAcceptsIndex =
    requires(const Collection& collection, Index index) { collection.contains(index); };

} // namespace

TEST_CASE("Contains free function operates on each supported lookup or indexed domain", "[contains]") {
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
    TRACE_ID("invariant.Contains.TestsApplicableLookupOrIndexedDomain");
    TRACE_ID("invariant.Contains.SupportsMapAndSetDeclaredLookupTypesOnly");
    TRACE_ID("invariant.Contains.SupportsVectorAndStringNonbooleanNoncharacterIntegerIndices");
    TRACE_ID("invariant.Contains.ExcludesRangeFreeFunction");
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
    STATIC_REQUIRE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, int>);
    STATIC_REQUIRE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, unsigned long long>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, bool>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, char>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, signed char>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, unsigned char>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, wchar_t>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, char8_t>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, char16_t>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<Vector<int, 4>, char32_t>);
    STATIC_REQUIRE_FALSE(ContainsMemberAcceptsIndex<Vector<int, 4>, bool>);
    STATIC_REQUIRE_FALSE(ContainsMemberAcceptsIndex<Vector<int, 4>, char>);
    STATIC_REQUIRE(cljonic::concepts_detail::ContainsAdmissible<Map<char, int, 4>, char>);
    STATIC_REQUIRE(cljonic::concepts_detail::ContainsAdmissible<Set<bool, 4>, bool>);

    // String: index in range.
    constexpr String<8> st{"abc"};
    STATIC_REQUIRE(contains(st, 0U));
    STATIC_REQUIRE(contains(st, 2U));
    STATIC_REQUIRE_FALSE(contains(st, 3U));
    STATIC_REQUIRE(cljonic::concepts_detail::ContainsAdmissible<String<8>, int>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<String<8>, bool>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::ContainsAdmissible<String<8>, char>);
    STATIC_REQUIRE_FALSE(ContainsMemberAcceptsIndex<String<8>, bool>);
    STATIC_REQUIRE_FALSE(ContainsMemberAcceptsIndex<String<8>, char>);

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