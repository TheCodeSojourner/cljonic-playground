#include <catch2/catch_test_macros.hpp>

#include <array>
#include <concepts>
#include <ranges>
#include <string_view>
#include <type_traits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Collection C++ interoperability exposes const logical traversal and views", "[interop]") {
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.CollectionCppInteroperability");
    TRACE_ID("invariant.CollectionCppInteroperability.SupportsConstTraversal");
    TRACE_ID("invariant.CollectionCppInteroperability.TraversalIsReadOnly");
    TRACE_ID("invariant.CollectionCppInteroperability.TraversalIsNonAllocating");
    TRACE_ID("invariant.CollectionCppInteroperability.TraversalIsNonThrowing");
    TRACE_ID("invariant.CollectionCppInteroperability.TraversalExposesActiveElementsOnly");
    TRACE_ID("invariant.CollectionCppInteroperability.VectorMapSetPreserveStorageOrder");
    TRACE_ID("invariant.CollectionCppInteroperability.QueuePreservesFifoOrder");
    TRACE_ID("invariant.CollectionCppInteroperability.QueueUsesLogicalTraversal");
    TRACE_ID("invariant.CollectionCppInteroperability.StringExcludesNullTerminator");
    TRACE_ID("invariant.CollectionCppInteroperability.ViewsAreConstAndNonOwning");

    constexpr auto vector = Vector<int, 4>{1, 2};
    constexpr auto map = Map<int, int, 2>{MapEntry<int, int>{1, 10}};
    constexpr auto set = Set<int, 3>{3, 1};
    constexpr auto string = String{"Hi"};
    constexpr auto queue = Queue<int, 4>{1, 2, 3, 4};

    STATIC_REQUIRE(std::ranges::forward_range<decltype(vector)>);
    STATIC_REQUIRE(std::ranges::forward_range<decltype(map)>);
    STATIC_REQUIRE(std::ranges::forward_range<decltype(set)>);
    STATIC_REQUIRE(std::ranges::forward_range<decltype(queue)>);
    STATIC_REQUIRE(std::ranges::forward_range<decltype(string)>);
    STATIC_REQUIRE(std::same_as<decltype(*vector.begin()), const int&>);
    STATIC_REQUIRE(std::same_as<decltype(*map.begin()), const MapEntry<int, int>&>);
    STATIC_REQUIRE(std::same_as<decltype(*string.begin()), const char&>);
    STATIC_REQUIRE(vector.view().size() == 2U);
    STATIC_REQUIRE(map.view().size() == 1U);
    STATIC_REQUIRE(set.view().size() == 2U);
    STATIC_REQUIRE(string.view() == std::string_view{"Hi"});
    auto wrapped = queue.pop().conj(5);
    REQUIRE(wrapped.begin() != wrapped.end());
    REQUIRE(*wrapped.begin() == 2);
    REQUIRE(*(++wrapped.begin()) == 3);
    REQUIRE(*(++(++wrapped.begin())) == 4);
    REQUIRE(*(++(++(++wrapped.begin()))) == 5);
    REQUIRE(wrapped.count() == 4U);

    const auto empty_vector = Vector<int, 4>{};
    const auto empty_map = Map<int, int, 2>{};
    const auto empty_set = Set<int, 3>{};
    const auto empty_string = String<0>{};
    REQUIRE(empty_vector.view().empty());
    REQUIRE(empty_map.view().empty());
    REQUIRE(empty_set.view().empty());
    REQUIRE(empty_string.view().empty());
}

TEST_CASE("Vector accepts bounded range and view source imports", "[interop][vector][range]") {
    using cljonic::Vector;

    std::array<int, 5> values{10, 20, 30, 40, 50};
    constexpr std::array<int, 3> static_values{{1, 2, 3}};
    const auto from_static_array = Vector<int, 3>{static_values};
    const auto from_static_span = Vector<int, 3>{std::span<const int, 3>{static_values}};
    REQUIRE(from_static_array.count() == 3U);
    REQUIRE(from_static_span.count() == 3U);

    const std::span<const int> dynamic_source{values};
    const auto from_array = Vector<int, 3>{dynamic_source};
    REQUIRE(from_array.count() == 3U);
    REQUIRE(from_array.view()[0] == 10);
    REQUIRE(from_array.view()[1] == 20);
    REQUIRE(from_array.view()[2] == 30);

    const auto mapped = std::views::transform(values, [](int value) { return value * 2; });
    const auto from_view = Vector<int, 4>{mapped};
    REQUIRE(from_view.count() == 4U);
    REQUIRE(from_view.view()[0] == 20);
    REQUIRE(from_view.view()[1] == 40);
    REQUIRE(from_view.view()[2] == 60);
    REQUIRE(from_view.view()[3] == 80);

    const auto from_view_fit = Vector<int, 5>{mapped};
    REQUIRE(from_view_fit.count() == 5U);
}
