#include <catch2/catch_test_macros.hpp>

#include <concepts>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Empty free function operations", "[empty]") {
    using cljonic::count;
    using cljonic::empty;
    using cljonic::is_empty;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Empty");
    TRACE_ID("invariant.Empty.EmptyFreeFunctionSupported");
    TRACE_ID("invariant.Empty.ReturnsEmptyOwningValueOfSameType");
    TRACE_ID("invariant.Empty.SupportsSequenceableCollectionDomain");
    TRACE_ID("invariant.Empty.SupportedCollectionsPreserveTypeAndCapacity");
    TRACE_ID("invariant.Empty.LeavesInputUnchanged");
    TRACE_ID("invariant.Empty.ResultSatisfiesIsEmpty");
    TRACE_ID("invariant.Empty.NoHeapAllocation");
    TRACE_ID("invariant.Empty.NoRtti");
    TRACE_ID("invariant.Empty.NoExceptions");
    TRACE_ID("invariant.Empty.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Empty.ReferentialTransparency");
    TRACE_ID("invariant.Empty.RejectsUnsupportedDomainWithTargetedDiagnostic");
    TRACE_ID("invariant.Empty.DiagnosticNamesSupportedCollections");

    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Vector<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Map<int, int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Set<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Queue<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<String<4>>);

    constexpr Vector<int, 4> v{1, 2, 3};
    constexpr auto ev = empty(v);
    STATIC_REQUIRE(std::same_as<decltype(ev), const Vector<int, 4>>);
    STATIC_REQUIRE(is_empty(ev));
    STATIC_REQUIRE(count(ev) == 0U);
    STATIC_REQUIRE(count(v) == 3U);
    STATIC_REQUIRE(v(0) == 1);

    constexpr Map<int, int, 4> m{MapEntry<int, int>{1, 100}};
    constexpr auto em = empty(m);
    STATIC_REQUIRE(std::same_as<decltype(em), const Map<int, int, 4>>);
    STATIC_REQUIRE(is_empty(em));
    STATIC_REQUIRE(count(m) == 1U);

    constexpr Set<int, 4> s{1, 2};
    constexpr auto es = empty(s);
    STATIC_REQUIRE(std::same_as<decltype(es), const Set<int, 4>>);
    STATIC_REQUIRE(is_empty(es));
    STATIC_REQUIRE(count(s) == 2U);

    constexpr Queue<int, 4> q{1};
    constexpr auto eq = empty(q);
    STATIC_REQUIRE(std::same_as<decltype(eq), const Queue<int, 4>>);
    STATIC_REQUIRE(is_empty(eq));
    STATIC_REQUIRE(count(q) == 1U);

    constexpr String<4> text{"AB"};
    constexpr auto etext = empty(text);
    STATIC_REQUIRE(std::same_as<decltype(etext), const String<4>>);
    STATIC_REQUIRE(is_empty(etext));
    STATIC_REQUIRE(count(text) == 2U);

    // Runtime tests for code coverage instrumentation
    auto rv = Vector<int, 4>{9};
    REQUIRE(is_empty(empty(rv)));
    REQUIRE(count(rv) == 1U);
    auto rm = Map<int, int, 4>{MapEntry<int, int>{1, 100}};
    REQUIRE(is_empty(empty(rm)));
    REQUIRE(count(rm) == 1U);
    auto rq = Queue<int, 4>{5};
    REQUIRE(is_empty(empty(rq)));
    REQUIRE(count(rq) == 1U);
    auto rtext = String<4>{"AB"};
    REQUIRE(is_empty(empty(rtext)));
    REQUIRE(count(rtext) == 2U);
}
