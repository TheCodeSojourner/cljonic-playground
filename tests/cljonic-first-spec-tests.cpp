#include <catch2/catch_test_macros.hpp>

#include <concepts>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {
template <typename T>
concept HasAdmittedFirst = requires(const T& value) { requires !std::same_as<decltype(cljonic::first(value)), void>; };
} // namespace

TEST_CASE("First free function operations", "[first]") {
    using cljonic::count;
    using cljonic::first;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.First");
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Vector<int, 4>>);

    TRACE_ID("invariant.First.FirstFreeFunctionSupported");
    STATIC_REQUIRE(std::same_as<decltype(first(Vector<int, 4>{10})), int>);

    TRACE_ID("invariant.First.ReturnsFirstLogicalElement");
    STATIC_REQUIRE(first(Vector<int, 4>{10, 20}) == 10);

    TRACE_ID("invariant.First.SupportsSequenceableCollectionDomain");
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Vector<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Map<int, int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Set<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Queue<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<String<4>>);

    TRACE_ID("invariant.First.SupportsMapEntryDomain");
    STATIC_REQUIRE(first(MapEntry<int, int>{1, 100}) == 1);

    TRACE_ID("invariant.First.VectorAndStringUseIndexZero");
    STATIC_REQUIRE(first(Vector<int, 4>{10, 20, 30}) == 10);
    STATIC_REQUIRE(first(String<4>{"AB"}) == 'A');

    TRACE_ID("invariant.First.QueueUsesFrontElement");
    STATIC_REQUIRE(first(Queue<int, 4>{5, 6}) == 5);

    TRACE_ID("invariant.First.MapReturnsValueSemanticEntry");
    STATIC_REQUIRE(first(Map<int, int, 4>{MapEntry<int, int>{1, 100}}) == MapEntry<int, int>{1, 100});

    TRACE_ID("invariant.First.SetReturnsStoredElement");
    STATIC_REQUIRE(first(Set<int, 4>{7}) == 7);

    TRACE_ID("invariant.First.MapEntryReturnsKey");
    STATIC_REQUIRE(first(MapEntry<int, int>{3, 300}) == 3);
    STATIC_REQUIRE(first(first(Map<int, int, 4>{MapEntry<int, int>{3, 300}})) == 3);

    TRACE_ID("invariant.First.EmptyYieldsDefaultElement");
    STATIC_REQUIRE(first(Vector<int, 4>{}) == 0);
    STATIC_REQUIRE(first(Map<int, int, 4>{}) == MapEntry<int, int>{});
    STATIC_REQUIRE(first(Set<int, 4>{}) == 0);
    STATIC_REQUIRE(first(Queue<int, 4>{}) == 0);
    STATIC_REQUIRE(first(String<4>{}) == '\0');

    TRACE_ID("invariant.First.MapAndSetTraversalOrderIsNotSemanticallySignificant");
    constexpr Map<int, int, 4> map_value{MapEntry<int, int>{1, 100}};
    constexpr Set<int, 4> set_value{7};
    STATIC_REQUIRE(first(map_value) == MapEntry<int, int>{1, 100});
    STATIC_REQUIRE(first(set_value) == 7);

    TRACE_ID("invariant.First.LeavesInputUnchanged");
    constexpr Vector<int, 4> source{10, 20};
    STATIC_REQUIRE(first(source) == 10);
    STATIC_REQUIRE(count(source) == 2U);
    STATIC_REQUIRE(source(0) == 10);

    TRACE_ID("invariant.First.NoHeapAllocation");
    STATIC_REQUIRE(noexcept(first(source)));

    TRACE_ID("invariant.First.NoRtti");
    STATIC_REQUIRE(noexcept(first(map_value)));

    TRACE_ID("invariant.First.NoExceptions");
    STATIC_REQUIRE(noexcept(first(set_value)));

    TRACE_ID("invariant.First.SingleThreadedExecutionModel");
    STATIC_REQUIRE(first(MapEntry<int, int>{4, 400}) == 4);

    TRACE_ID("invariant.First.ReferentialTransparency");
    STATIC_REQUIRE(first(Vector<int, 4>{10, 20}) == first(Vector<int, 4>{10, 20}));

    TRACE_ID("invariant.First.RejectsUnsupportedDomainWithTargetedDiagnostic");
    STATIC_REQUIRE_FALSE(HasAdmittedFirst<int>);

    TRACE_ID("invariant.First.DiagnosticNamesSupportedDomains");
    STATIC_REQUIRE(HasAdmittedFirst<Vector<int, 4>>);
    STATIC_REQUIRE(HasAdmittedFirst<Map<int, int, 4>>);
    STATIC_REQUIRE(HasAdmittedFirst<Set<int, 4>>);
    STATIC_REQUIRE(HasAdmittedFirst<Queue<int, 4>>);
    STATIC_REQUIRE(HasAdmittedFirst<String<4>>);
    STATIC_REQUIRE(HasAdmittedFirst<MapEntry<int, int>>);

    volatile int vector_value_raw = 42;
    int vector_value = vector_value_raw;
    auto rv = Vector<int, 4>{vector_value, 20};
    REQUIRE(first(rv) == 42);
    REQUIRE(count(rv) == 2U);

    volatile int map_key_raw = 5;
    volatile int map_mapped_value_raw = 500;
    int map_key = map_key_raw;
    int map_mapped_value = map_mapped_value_raw;
    auto rm = Map<int, int, 4>{MapEntry<int, int>{map_key, map_mapped_value}};
    REQUIRE(first(rm) == MapEntry<int, int>{5, 500});
    REQUIRE(count(rm) == 1U);

    auto rs = Set<int, 4>{7};
    REQUIRE(first(rs) == 7);

    volatile int queue_value_raw = 11;
    int queue_value = queue_value_raw;
    auto rq = Queue<int, 4>{queue_value, 12};
    REQUIRE(first(rq) == 11);

    auto rtext = String<4>{"AB"};
    REQUIRE(first(rtext) == 'A');

    auto re = MapEntry<int, int>{6, 600};
    REQUIRE(first(re) == 6);

    auto ev = Vector<int, 4>{};
    REQUIRE(first(ev) == 0);
    auto em = Map<int, int, 4>{};
    REQUIRE(first(em) == MapEntry<int, int>{});
    auto es = Set<int, 4>{};
    REQUIRE(first(es) == 0);
    auto eq = Queue<int, 4>{};
    REQUIRE(first(eq) == 0);
    auto etext = String<4>{};
    REQUIRE(first(etext) == '\0');
}
