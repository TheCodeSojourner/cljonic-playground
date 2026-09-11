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
