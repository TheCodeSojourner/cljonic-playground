#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#include <array>
#include <concepts>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("IsEmpty free function operations", "[is_empty]") {
    using cljonic::conj;
    using cljonic::count;
    using cljonic::is_empty;
    using cljonic::Queue;
    using cljonic::Range;
    using cljonic::Repeat;
    using cljonic::Vector;

    TRACE_ID("entity-fields.IsEmpty");
    TRACE_ID("invariant.IsEmpty.IsEmptyFreeFunctionSupported");
    TRACE_ID("invariant.IsEmpty.IsEmptyDerivedFromZeroCount");
    TRACE_ID("invariant.IsEmpty.NoCollectionIsEmptyMember");
    TRACE_ID("invariant.IsEmpty.ReturnsBooleanPredicate");
    TRACE_ID("invariant.IsEmpty.SupportsSequenceableCollectionDomain");
    TRACE_ID("invariant.IsEmpty.SupportsSequenceableProducerDomain");
    TRACE_ID("invariant.IsEmpty.ZeroCountMatchesEmptyObservation");
    TRACE_ID("invariant.IsEmpty.FollowsProducerCountObservation");
    TRACE_ID("invariant.IsEmpty.RejectsUnsupportedDomainWithTargetedDiagnostic");
    TRACE_ID("invariant.IsEmpty.DiagnosticNamesRejectedOperandAndRequiredSequenceableDomain");
    TRACE_ID("invariant.IsEmpty.NoHeapAllocation");
    TRACE_ID("invariant.IsEmpty.NoRtti");
    TRACE_ID("invariant.IsEmpty.NoExceptions");
    TRACE_ID("invariant.IsEmpty.SingleThreadedExecutionModel");
    TRACE_ID("invariant.IsEmpty.ReferentialTransparency");
    TRACE_ID("invariant.IsEmpty.SupportsConstantEvaluation");
    TRACE_ID("invariant.IsEmpty.IsNoexcept");

    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableCollection<int>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableProducer<int>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableCollection<std::array<int, 4>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableProducer<std::array<int, 4>>);
    STATIC_REQUIRE(std::same_as<decltype(is_empty(Vector<int, 4>{})), bool>);
    STATIC_REQUIRE(std::same_as<decltype(is_empty(Range<int>{0, 0})), bool>);
    STATIC_REQUIRE(noexcept(is_empty(Vector<int, 4>{})));
    STATIC_REQUIRE(noexcept(is_empty(Range<int>{0, 0})));
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Vector<int, 4>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableCollection<Range<int>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableProducer<Vector<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableProducer<Range<int>>);
    STATIC_REQUIRE(is_empty(Range<int>{0, 0}));
    STATIC_REQUIRE(is_empty(Range<int>{4, 0}));
    STATIC_REQUIRE_FALSE(is_empty(Range<int>{0, 3}));
    STATIC_REQUIRE_FALSE(is_empty(Range<int>{0, 0, 0}));
    STATIC_REQUIRE_FALSE(is_empty(Repeat<int>{1}));

    constexpr Vector<int, 4> e{};
    constexpr Vector<int, 4> v{1};
    STATIC_REQUIRE(is_empty(e));
    STATIC_REQUIRE(is_empty(e) == (count(e) == 0U));
    STATIC_REQUIRE_FALSE(is_empty(v));
    STATIC_REQUIRE(is_empty(v) == (count(v) == 0U));

    constexpr Queue<int, 4> q{};
    constexpr auto q1 = conj(q, 5);
    STATIC_REQUIRE(is_empty(q));
    STATIC_REQUIRE(is_empty(q) == (count(q) == 0U));
    STATIC_REQUIRE_FALSE(is_empty(q1));

    volatile int raw = 9;
    const int value = raw;
    auto rv = Vector<int, 4>{value};
    REQUIRE_FALSE(is_empty(rv));
    auto rev = Vector<int, 4>{};
    REQUIRE(is_empty(rev));
    REQUIRE(is_empty(Range<int>{value, value}));
    REQUIRE_FALSE(is_empty(Range<int>{0, value}));
    REQUIRE_FALSE(is_empty(Repeat<int>{value}));
}

TEST_CASE("IsEmpty observes every owning collection without changing it", "[is_empty][collection]") {
    using namespace cljonic;

    TRACE_ID("invariant.IsEmpty.LeavesInputUnchanged");

    volatile int raw = 7;
    const int value = raw;

    SECTION("Vector") {
        constexpr auto vacant = Vector<int, 2>{};
        constexpr auto full = Vector<int, 2>{0, 0};
        STATIC_REQUIRE(is_empty(vacant));
        STATIC_REQUIRE_FALSE(is_empty(full));
        STATIC_REQUIRE(noexcept(is_empty(full)));
        const auto values = Vector<int, 2>{value, value};
        const auto original = values;
        REQUIRE_FALSE(is_empty(values));
        REQUIRE(is_empty(values) == (count(values) == 0));
        REQUIRE(equal(values, original));
        REQUIRE(is_empty(vacant));
    }

    SECTION("Map") {
        constexpr auto vacant = Map<int, int, 1>{};
        constexpr auto full = Map<int, int, 1>{MapEntry<int, int>{0, 0}};
        STATIC_REQUIRE(is_empty(vacant));
        STATIC_REQUIRE_FALSE(is_empty(full));
        STATIC_REQUIRE(noexcept(is_empty(full)));
        const auto values = Map<int, int, 1>{MapEntry<int, int>{value, value}};
        const auto original = values;
        REQUIRE_FALSE(is_empty(values));
        REQUIRE(is_empty(values) == (count(values) == 0));
        REQUIRE(equal(values, original));
        REQUIRE(is_empty(vacant));
    }

    SECTION("Set") {
        constexpr auto vacant = Set<int, 1>{};
        constexpr auto full = Set<int, 1>{0};
        STATIC_REQUIRE(is_empty(vacant));
        STATIC_REQUIRE_FALSE(is_empty(full));
        STATIC_REQUIRE(noexcept(is_empty(full)));
        const auto values = Set<int, 1>{value};
        const auto original = values;
        REQUIRE_FALSE(is_empty(values));
        REQUIRE(is_empty(values) == (count(values) == 0));
        REQUIRE(equal(values, original));
        REQUIRE(is_empty(vacant));
    }

    SECTION("Queue") {
        constexpr auto vacant = Queue<int, 2>{};
        constexpr auto full = Queue<int, 2>{0, 0};
        STATIC_REQUIRE(is_empty(vacant));
        STATIC_REQUIRE_FALSE(is_empty(full));
        STATIC_REQUIRE(noexcept(is_empty(full)));
        const auto values = Queue<int, 2>{value, value};
        const auto original = values;
        REQUIRE_FALSE(is_empty(values));
        REQUIRE(is_empty(values) == (count(values) == 0));
        REQUIRE(equal(values, original));
        REQUIRE(is_empty(vacant));
    }

    SECTION("String") {
        constexpr auto vacant = String<2>{};
        constexpr auto full = String<2>{"AB"};
        STATIC_REQUIRE(is_empty(vacant));
        STATIC_REQUIRE_FALSE(is_empty(full));
        STATIC_REQUIRE(noexcept(is_empty(full)));
        const char source[]{static_cast<char>('A' + value), '\0'};
        const auto values = String<2>{source};
        const auto original = values;
        REQUIRE_FALSE(is_empty(values));
        REQUIRE(is_empty(values) == (count(values) == 0));
        REQUIRE(equal(values, original));
        REQUIRE(is_empty(vacant));
    }
}

TEST_CASE("IsEmpty treats zero-capacity collections as empty", "[is_empty][boundary]") {
    using namespace cljonic;

    STATIC_REQUIRE(is_empty(Vector<int, 0>{}));
    STATIC_REQUIRE(is_empty(Map<int, int, 0>{}));
    STATIC_REQUIRE(is_empty(Set<int, 0>{}));
    STATIC_REQUIRE(is_empty(Queue<int, 0>{}));
    STATIC_REQUIRE(is_empty(String<0>{}));
    REQUIRE(is_empty(Vector<int, 0>{}));
    REQUIRE(is_empty(Map<int, int, 0>{}));
    REQUIRE(is_empty(Set<int, 0>{}));
    REQUIRE(is_empty(Queue<int, 0>{}));
    REQUIRE(is_empty(String<0>{}));
}

TEST_CASE("IsEmpty follows finite and unbounded producer counts without changing parameters", "[is_empty][producer]") {
    using namespace cljonic;

    TRACE_ID("invariant.IsEmpty.LeavesInputUnchanged");

    volatile int raw = 3;
    const int value = raw;
    volatile std::size_t raw_zero = 0;
    const std::size_t zero = raw_zero;
    const auto cap_is_zero = CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE == 0;

    SECTION("Range") {
        STATIC_REQUIRE(is_empty(Range{0, 0}));
        STATIC_REQUIRE(is_empty(Range{3, 0}));
        STATIC_REQUIRE(is_empty(Range{0, 3, -1}));
        STATIC_REQUIRE_FALSE(is_empty(Range{0, 3}));
        STATIC_REQUIRE_FALSE(is_empty(Range{3, 0, -1}));
        STATIC_REQUIRE(is_empty(Range{0, 0, 0}) == (CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE == 0));
        const auto vacant = Range{value, value};
        const auto ascending = Range{0, value};
        const auto descending = Range{value, 0, -1};
        const auto unbounded = Range{value, value, 0};
        const auto original = ascending;
        REQUIRE(is_empty(vacant));
        REQUIRE_FALSE(is_empty(ascending));
        REQUIRE_FALSE(is_empty(descending));
        REQUIRE(is_empty(unbounded) == cap_is_zero);
        REQUIRE(is_empty(ascending) == (count(ascending) == 0));
        REQUIRE(ascending == original);
    }

    SECTION("Repeat") {
        STATIC_REQUIRE(is_empty(Repeat{std::size_t{0}, 7}));
        STATIC_REQUIRE_FALSE(is_empty(Repeat{std::size_t{2}, 7}));
        STATIC_REQUIRE(noexcept(is_empty(Repeat{7})));
        const auto vacant = Repeat{zero, value};
        const auto finite = Repeat{std::size_t{2}, value};
        const auto unbounded = Repeat{value};
        const auto original = finite;
        REQUIRE(is_empty(vacant));
        REQUIRE_FALSE(is_empty(finite));
        REQUIRE(is_empty(unbounded) == cap_is_zero);
        REQUIRE(is_empty(finite) == (count(finite) == 0));
        REQUIRE(finite == original);
    }

    SECTION("Cycle") {
        // An empty-source Cycle still reports the unbounded count observation cap.
        constexpr auto vacant = cycle(Vector<int, 2>{});
        constexpr auto unbounded = cycle(Vector<int, 2>{1});
        STATIC_REQUIRE(is_empty(vacant) == (CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE == 0));
        STATIC_REQUIRE(is_empty(unbounded) == (CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE == 0));
        STATIC_REQUIRE(noexcept(is_empty(unbounded)));
        const auto runtime_vacant = cycle(Range{value, value});
        const auto runtime_unbounded = cycle(Vector<int, 2>{value});
        const auto original = runtime_unbounded;
        REQUIRE(is_empty(runtime_vacant) == cap_is_zero);
        REQUIRE(is_empty(runtime_unbounded) == cap_is_zero);
        REQUIRE(is_empty(runtime_unbounded) == (count(runtime_unbounded) == 0));
        REQUIRE(runtime_unbounded == original);
    }

    SECTION("Iterate") {
        constexpr auto step = [](int item) constexpr noexcept { return item + 1; };
        constexpr auto unbounded = iterate(step, 0);
        STATIC_REQUIRE(is_empty(unbounded) == (CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE == 0));
        STATIC_REQUIRE(noexcept(is_empty(unbounded)));
        const auto runtime_unbounded = iterate(step, value);
        const auto original = runtime_unbounded;
        REQUIRE(is_empty(runtime_unbounded) == cap_is_zero);
        REQUIRE(is_empty(runtime_unbounded) == (count(runtime_unbounded) == 0));
        REQUIRE(count(runtime_unbounded) == count(original));
        REQUIRE(*runtime_unbounded.begin() == *original.begin());
    }

    SECTION("Repeatedly") {
        constexpr auto step = [] constexpr noexcept { return 7; };
        STATIC_REQUIRE(is_empty(repeatedly(std::size_t{0}, step)));
        STATIC_REQUIRE_FALSE(is_empty(repeatedly(std::size_t{2}, step)));
        STATIC_REQUIRE(noexcept(is_empty(repeatedly(step))));
        const auto runtime_step = [value] noexcept { return value; };
        const auto vacant = repeatedly(zero, runtime_step);
        const auto finite = repeatedly(std::size_t{2}, runtime_step);
        const auto unbounded = repeatedly(runtime_step);
        const auto original = finite;
        REQUIRE(is_empty(vacant));
        REQUIRE_FALSE(is_empty(finite));
        REQUIRE(is_empty(unbounded) == cap_is_zero);
        REQUIRE(is_empty(finite) == (count(finite) == 0));
        REQUIRE(count(finite) == count(original));
        REQUIRE(*finite.begin() == *original.begin());
    }
}