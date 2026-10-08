#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Count free function operations", "[count]") {
    using cljonic::assoc;
    using cljonic::conj;
    using cljonic::count;
    using cljonic::Map;
    using cljonic::Queue;
    using cljonic::Range;
    using cljonic::Repeat;
    using cljonic::Set;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Count");
    TRACE_ID("invariant.Count.CountFreeFunctionSupported");
    TRACE_ID("invariant.Count.SupportsSequenceableCollectionDomain");
    TRACE_ID("invariant.Count.SupportsSequenceableProducerDomain");
    TRACE_ID("invariant.Count.ReturnsExactCollectionCount");
    TRACE_ID("invariant.Count.RejectsUnsupportedDomainWithTargetedDiagnostic");
    TRACE_ID("invariant.Count.NoHeapAllocation");
    TRACE_ID("invariant.Count.NoRtti");
    TRACE_ID("invariant.Count.NoExceptions");
    TRACE_ID("invariant.Count.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Count.ReferentialTransparency");
    TRACE_ID("invariant.Count.ReturnsLogicalSize");

    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableCollection<int>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableProducer<int>);

    constexpr Vector<int, 4> v{1, 2, 3};
    STATIC_REQUIRE(count(v) == 3U);

    constexpr Set<int, 4> s{};
    constexpr auto s1 = conj(s, 10);
    STATIC_REQUIRE(count(s1) == 1U);

    constexpr Map<int, int, 4> m{};
    constexpr auto m1 = assoc(m, 1, 2);
    STATIC_REQUIRE(count(m1) == 1U);

    constexpr Queue<int, 4> q{};
    constexpr auto q1 = conj(q, 7);
    STATIC_REQUIRE(count(q1) == 1U);

    STATIC_REQUIRE(count(s) == 0U); // empty input
    STATIC_REQUIRE(count(m) == 0U);
    STATIC_REQUIRE(count(q) == 0U);
    // Runtime tests for code coverage instrumentation
    volatile int val_raw = 42;
    int val = val_raw;
    auto rv = Vector<int, 4>{1, 2};
    REQUIRE(count(rv) == 2U);
    auto rs = Set<int, 4>{};
    REQUIRE(count(conj(rs, val)) == 1U);
}

TEST_CASE("Count free function reports producer observations", "[count][producer]") {
    using cljonic::count;
    using cljonic::cycle;
    using cljonic::iterate;
    using cljonic::Range;
    using cljonic::Repeat;
    using cljonic::repeatedly;

    TRACE_ID("invariant.Count.FiniteProducerCountIsExact");
    TRACE_ID("invariant.Count.UnboundedProducerCountIsObservableCap");

    constexpr auto finite_range = Range{0, 5};
    constexpr auto finite_repeat = Repeat{7, 3U};
    constexpr auto finite_repeatedly = repeatedly(3U, []() constexpr noexcept { return 42; });
    STATIC_REQUIRE(count(finite_range) == 5U);
    STATIC_REQUIRE(count(finite_repeat) == 3U);
    STATIC_REQUIRE(count(finite_repeatedly) == 3U);

    constexpr auto observation_cap = cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE;
    constexpr auto unbounded_range = Range{0, 0, 0};
    constexpr auto unbounded_repeat = Repeat{7};
    constexpr auto unbounded_cycle = cycle(Range{1, 4});
    constexpr auto unbounded_iterate = iterate([](int value) constexpr noexcept { return value + 1; }, 0);
    constexpr auto unbounded_repeatedly = repeatedly([]() constexpr noexcept { return 42; });
    STATIC_REQUIRE(count(unbounded_range) == observation_cap);
    STATIC_REQUIRE(count(unbounded_repeat) == observation_cap);
    STATIC_REQUIRE(count(unbounded_cycle) == observation_cap);
    STATIC_REQUIRE(count(unbounded_iterate) == observation_cap);
    STATIC_REQUIRE(count(unbounded_repeatedly) == observation_cap);

    const auto runtime_producer = Range{2, 6};
    REQUIRE(count(runtime_producer) == 4U);
}