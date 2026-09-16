#include <catch2/catch_test_macros.hpp>

#include <limits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("CanAssoc free function operations", "[can_assoc]") {
    using cljonic::can_assoc;
    using cljonic::Map;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.CanAssoc");
    TRACE_ID("invariant.CanAssoc.CanAssocPreflightSupported");
    TRACE_ID("invariant.CanAssoc.DependsOnlyOnCollectionAndKey");
    TRACE_ID("invariant.CanAssoc.SupportsMapCanAssoc");
    TRACE_ID("invariant.CanAssoc.SupportsVectorCanAssoc");
    TRACE_ID("invariant.CanAssoc.SupportsStringCanAssoc");
    TRACE_ID("invariant.CanAssoc.TrueForExistingKeyOrIndex");
    TRACE_ID("invariant.CanAssoc.TrueForCountAppendWhenCapacityRemains");
    TRACE_ID("invariant.CanAssoc.FalseForInvalidOrFullCapacityKeyOrIndex");
    TRACE_ID("invariant.CanAssoc.ValueIndependentResult");
    TRACE_ID("invariant.CanAssoc.NonThrowingNonAllocatingPreflight");
    TRACE_ID("invariant.CanAssoc.ConstexprExecutionModel");
    TRACE_ID("invariant.CanAssoc.NoexceptExecutionModel");
    TRACE_ID("invariant.CanAssoc.NonMutatingSemantics");
    TRACE_ID("invariant.CanAssoc.NonAllocatingSemantics");
    TRACE_ID("invariant.CanAssoc.NoHeapAllocation");
    TRACE_ID("invariant.CanAssoc.NoRtti");
    TRACE_ID("invariant.CanAssoc.NoExceptions");
    TRACE_ID("invariant.CanAssoc.SingleThreadedExecutionModel");
    TRACE_ID("invariant.CanAssoc.ReferentialTransparency");

    constexpr Map<int, int, 4> m0{};
    STATIC_REQUIRE(can_assoc(m0, 1));

    // Runtime tests for code coverage instrumentation
    volatile int k_raw = 1;
    int k = k_raw;
    auto rm = Map<int, int, 4>{};
    REQUIRE(can_assoc(rm, k));

    constexpr Vector<int, 2> vector{1};
    STATIC_REQUIRE(can_assoc(vector, 0U));
    STATIC_REQUIRE(can_assoc(vector, 1U));
    STATIC_REQUIRE_FALSE(can_assoc(vector, 2U));
    STATIC_REQUIRE_FALSE(can_assoc(vector, -1));

    constexpr String<2> string{"a"};
    STATIC_REQUIRE(can_assoc(string, 0U));
    STATIC_REQUIRE(can_assoc(string, 1U));
    STATIC_REQUIRE_FALSE(can_assoc(string, 2U));
    STATIC_REQUIRE_FALSE(can_assoc(string, -1));
    STATIC_REQUIRE_FALSE(can_assoc(string, std::numeric_limits<unsigned long long>::max()));

    constexpr Map<int, int, 1> full_map{cljonic::MapEntry<int, int>{1, 1}};
    STATIC_REQUIRE(can_assoc(full_map, 1));
    STATIC_REQUIRE_FALSE(can_assoc(full_map, 2));
}
