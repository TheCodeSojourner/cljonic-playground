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
    TRACE_ID("invariant.CanAssoc.SupportsVariadicPreflight");
    TRACE_ID("invariant.CanAssoc.VariadicPreflightFoldsPairsLeftToRight");
    TRACE_ID("invariant.CanAssoc.VariadicPreflightUsesAccumulatedState");
    TRACE_ID("invariant.CanAssoc.VariadicPreflightFalseIfAnyPairFails");
    TRACE_ID("invariant.CanAssoc.VariadicPairsAdmittedRegardlessOfPosition");
    TRACE_ID("invariant.CanAssoc.OddTrailingArgumentRejected");
    TRACE_ID("invariant.CanAssoc.VariadicPreflightPreservesSource");
    TRACE_ID("invariant.CanAssoc.ChecksCompleteAssocArguments");
    TRACE_ID("invariant.CanAssoc.SupportsMapCanAssoc");
    TRACE_ID("invariant.CanAssoc.SupportsVectorCanAssoc");
    TRACE_ID("invariant.CanAssoc.SupportsStringCanAssoc");
    TRACE_ID("invariant.CanAssoc.TrueForExistingKeyOrIndex");
    TRACE_ID("invariant.CanAssoc.TrueForCountAppendWhenCapacityRemains");
    TRACE_ID("invariant.CanAssoc.FalseForInvalidOrFullCapacityKeyOrIndex");
    TRACE_ID("invariant.CanAssoc.FalseForInvalidStringCharacter");
    TRACE_ID("invariant.Map.CanAssocUsesSuppliedAssociationValue");
    TRACE_ID("invariant.Vector.CanAssocAcceptsAssociationValue");
    TRACE_ID("invariant.String.CanAssocFalseForInvalidCharacter");
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
    STATIC_REQUIRE(can_assoc(m0, 1, 10));
    STATIC_REQUIRE(can_assoc(Map<int, int, 2>{}, 1, 10, 2, 20));
    STATIC_REQUIRE_FALSE(can_assoc(Map<int, int, 1>{cljonic::MapEntry<int, int>{1, 10}}, 1, 20, 2, 30));
    STATIC_REQUIRE(can_assoc(Map<int, int, 1>{cljonic::MapEntry<int, int>{1, 10}}, 1, 20, 1, 30));

    // Runtime tests for code coverage instrumentation
    volatile int k_raw = 1;
    int k = k_raw;
    auto rm = Map<int, int, 4>{};
    REQUIRE(can_assoc(rm, k, 100));

    constexpr Vector<int, 2> vector{1};
    STATIC_REQUIRE(can_assoc(vector, 0U, 5));
    STATIC_REQUIRE(can_assoc(vector, 1U, 6));
    STATIC_REQUIRE_FALSE(can_assoc(vector, 2U, 7));
    STATIC_REQUIRE_FALSE(can_assoc(vector, -1, 8));
    STATIC_REQUIRE(can_assoc(Vector<int, 2>{1}, 0, 9, 1, 2));
    STATIC_REQUIRE_FALSE(can_assoc(Vector<int, 2>{1}, 1, 2, 2, 3));

    constexpr String<2> string{"a"};
    STATIC_REQUIRE(can_assoc(string, 0U, 'x'));
    STATIC_REQUIRE(can_assoc(string, 1U, 'y'));
    STATIC_REQUIRE_FALSE(can_assoc(string, 2U, 'z'));
    STATIC_REQUIRE_FALSE(can_assoc(string, -1, 'w'));
    STATIC_REQUIRE_FALSE(can_assoc(string, std::numeric_limits<unsigned long long>::max(), 'v'));
    STATIC_REQUIRE_FALSE(can_assoc(string, 0U, '\0'));
    STATIC_REQUIRE_FALSE(can_assoc(string, 0U, static_cast<char>(0x80)));
    STATIC_REQUIRE(can_assoc(String<2>{"a"}, 1, 'b', 0, 'x'));
    STATIC_REQUIRE_FALSE(can_assoc(String<2>{"a"}, 1, 'b', 2, 'c'));
    STATIC_REQUIRE_FALSE(can_assoc(String<3>{"a"}, 1, 'b', 0, '\0'));

    constexpr Map<int, int, 1> full_map{cljonic::MapEntry<int, int>{1, 1}};
    STATIC_REQUIRE(can_assoc(full_map, 1, 2));
    STATIC_REQUIRE_FALSE(can_assoc(full_map, 2, 3));
    REQUIRE(can_assoc(Map<int, int, 4>{}, 1, 10, 2, 20));
    REQUIRE_FALSE(can_assoc(Map<int, int, 1>{cljonic::MapEntry<int, int>{1, 10}}, 1, 20, 2, 30));
}
