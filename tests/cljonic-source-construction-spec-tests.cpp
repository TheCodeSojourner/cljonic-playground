#include <array>
#include <ranges>
#include <span>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Vector source construction preserves bounded ownership policy", "[source-construction]") {
    using cljonic::Vector;

    TRACE_ID("entity-fields.CollectionSourceConstruction");
    TRACE_ID("invariant.CollectionSourceConstruction.DirectSourceConstructionIsOwned");
    TRACE_ID("invariant.CollectionSourceConstruction.DirectSourceConstructionIsNonAllocating");
    TRACE_ID("invariant.CollectionSourceConstruction.DirectSourceConstructionIsNonThrowing");
    TRACE_ID("invariant.CollectionSourceConstruction.DirectSourceConstructionDoesNotBorrowSource");
    TRACE_ID("invariant.CollectionSourceConstruction.RangeOrViewSourceIsAccepted");
    TRACE_ID("invariant.CollectionSourceConstruction.StaticExtentOverflowIsCompileTimeFailure");
    TRACE_ID("invariant.CollectionSourceConstruction.RuntimeExtentOverflowReturnsBoundedPrefix");
    TRACE_ID("invariant.CollectionSourceConstruction.CompleteMaterializationUsesFitsInto");
    TRACE_ID("invariant.CollectionSourceConstruction.CompleteMaterializationUsesInto");
    TRACE_ID("invariant.CollectionSourceConstruction.CheckedConstructorIsNotRequired");

    static constexpr int source_values[] = {11, 22, 33};
    constexpr std::span<const int> source_view{source_values};
    constexpr auto copied = Vector<int, 4>{source_view};
    STATIC_REQUIRE(std::is_nothrow_constructible_v<Vector<int, 4>, std::span<const int>>);
    STATIC_REQUIRE(copied.count() == 3U);
    STATIC_REQUIRE(copied(0U) == 11);
    STATIC_REQUIRE(copied(2U) == 33);

    int runtime_values[] = {100, 200, 300};
    std::span<const int> runtime_view{runtime_values};
    const auto bounded = Vector<int, 2>{runtime_view};
    runtime_values[0] = -1;

    REQUIRE(bounded.count() == 2U);
    REQUIRE(bounded(0U) == 100);
    REQUIRE(bounded(1U) == 200);
    REQUIRE(runtime_view[0] == -1);
}

TEST_CASE("Map, Set, and Queue accept bounded span sources", "[source-construction]") {
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::Set;

    static constexpr MapEntry<int, int> map_entries[] = {{1, 10}, {2, 20}, {1, 30}, {3, 40}};
    constexpr std::span<const MapEntry<int, int>> map_source{map_entries};
    constexpr auto copied_map = Map<int, int, 4>{map_source};
    STATIC_REQUIRE(copied_map.count() == 3U);
    STATIC_REQUIRE(copied_map(1) == 30);
    STATIC_REQUIRE(copied_map(2) == 20);
    STATIC_REQUIRE(copied_map(3) == 40);
    constexpr auto copied_static_map = Map<int, int, 4>{std::span<const MapEntry<int, int>, 4>{map_entries}};
    STATIC_REQUIRE(copied_static_map.count() == 3U);

    static constexpr int set_values[] = {7, 9, 7, 11};
    constexpr std::span<const int> set_source{set_values};
    constexpr auto copied_set = Set<int, 4>{set_source};
    STATIC_REQUIRE(copied_set.count() == 3U);
    STATIC_REQUIRE(copied_set.contains(7));
    STATIC_REQUIRE(copied_set.contains(9));
    STATIC_REQUIRE(copied_set.contains(11));
    constexpr auto copied_static_set = Set<int, 4>{std::span<const int, 4>{set_values}};
    STATIC_REQUIRE(copied_static_set.count() == 3U);

    static constexpr int queue_values[] = {1, 2, 3, 4};
    constexpr std::span<const int> queue_source{queue_values};
    constexpr auto copied_queue = Queue<int, 3>{queue_source};
    STATIC_REQUIRE(copied_queue.count() == 3U);
    STATIC_REQUIRE(copied_queue.peek() == 1);
    STATIC_REQUIRE(copied_queue.begin()[2] == 3);
    constexpr auto copied_static_queue = Queue<int, 4>{std::span<const int, 4>{queue_values}};
    STATIC_REQUIRE(copied_static_queue.count() == 4U);

    int runtime_values[] = {100, 200, 300, 400};
    std::span<const int> runtime_source{runtime_values};
    const auto bounded_queue = Queue<int, 2>{runtime_source};
    runtime_values[0] = -1;

    REQUIRE(bounded_queue.count() == 2U);
    REQUIRE(bounded_queue.peek() == 100);
    REQUIRE(bounded_queue.begin()[1] == 200);
    REQUIRE(runtime_source[0] == -1);

    // Fixed-extent span CTAD
    constexpr std::span span_ctad_set{set_values};
    constexpr auto ctad_set = Set{span_ctad_set};
    STATIC_REQUIRE(std::same_as<decltype(ctad_set), const Set<int, 4>>);
    STATIC_REQUIRE(ctad_set.count() == 3U);

    int mutable_set_values[] = {7, 9, 11};
    std::span mutable_set_span{mutable_set_values};
    const auto ctad_mutable_set = Set{mutable_set_span};
    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(ctad_mutable_set)>, Set<int, 3>>);

    constexpr std::span span_ctad_map{map_entries};
    constexpr auto ctad_map = Map{span_ctad_map};
    STATIC_REQUIRE(std::same_as<decltype(ctad_map), const Map<int, int, 4>>);
    STATIC_REQUIRE(ctad_map.count() == 3U);

    constexpr std::span span_ctad_queue{queue_values};
    constexpr auto ctad_queue = Queue{span_ctad_queue};
    STATIC_REQUIRE(std::same_as<decltype(ctad_queue), const Queue<int, 4>>);
    STATIC_REQUIRE(ctad_queue.count() == 4U);

    int mutable_queue_values[] = {1, 2, 3};
    std::span mutable_queue_span{mutable_queue_values};
    const auto ctad_mutable_queue = Queue{mutable_queue_span};
    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(ctad_mutable_queue)>, Queue<int, 3>>);
}

TEST_CASE("Map, Set, and Queue accept bounded range and view sources", "[source-construction][range]") {
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::Set;

    const std::array<MapEntry<int, int>, 4> map_entries{{{1, 10}, {2, 20}, {1, 30}, {3, 40}}};
    const std::span<const MapEntry<int, int>> dynamic_map_source{map_entries};
    const auto copied_map = Map<int, int, 3>{dynamic_map_source};
    REQUIRE(copied_map.count() == 2U);
    REQUIRE(copied_map(1) == 30);
    REQUIRE(copied_map(2) == 20);
    REQUIRE_FALSE(copied_map.contains(3));

    const auto map_view = std::views::transform(map_entries, [](MapEntry<int, int> entry) { return entry; });
    const auto copied_map_view_fit = Map<int, int, 4>{map_view};
    REQUIRE(copied_map_view_fit.count() == 3U);
    const auto copied_map_view_bounded = Map<int, int, 3>{map_view};
    REQUIRE_FALSE(copied_map_view_bounded.contains(3));

    const std::array<int, 4> values{{1, 2, 3, 4}};
    const std::span<const int> dynamic_set_source{values};
    const auto copied_set_fit = Set<int, 4>{dynamic_set_source};
    REQUIRE(copied_set_fit.count() == 4U);
    const auto copied_set_bounded = Set<int, 2>{dynamic_set_source};
    REQUIRE(copied_set_bounded.count() == 2U);

    const auto doubled = std::views::transform(values, [](int value) { return value * 2; });
    const auto copied_set = Set<int, 3>{doubled};
    REQUIRE(copied_set.count() == 3U);
    REQUIRE(copied_set.contains(2));
    REQUIRE(copied_set.contains(4));
    REQUIRE(copied_set.contains(6));
    const auto copied_set_view_fit = Set<int, 4>{doubled};
    REQUIRE(copied_set_view_fit.count() == 4U);

    const auto tripled = std::views::transform(values, [](int value) { return value * 3; });
    const auto copied_queue = Queue<int, 2>{tripled};
    REQUIRE(copied_queue.count() == 2U);
    REQUIRE(copied_queue.peek() == 3);
    REQUIRE(copied_queue.begin()[1] == 6);
    const auto copied_queue_view_fit = Queue<int, 4>{tripled};
    REQUIRE(copied_queue_view_fit.count() == 4U);
}
