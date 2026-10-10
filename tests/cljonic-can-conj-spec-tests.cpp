#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("CanConj free function operations", "[can_conj]") {
    using cljonic::assoc;
    using cljonic::can_conj;
    using cljonic::conj;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::Set;
    using cljonic::Vector;

    TRACE_ID("entity-fields.CanConj");
    TRACE_ID("invariant.CanConj.CanConjPreflightSupported");
    TRACE_ID("invariant.CanConj.SupportsZeroValuePreflight");
    TRACE_ID("invariant.CanConj.ZeroValuePreflightIsTrue");
    TRACE_ID("invariant.CanConj.SupportsVariadicPreflight");
    TRACE_ID("invariant.CanConj.VariadicPreflightFoldsLeftToRight");
    TRACE_ID("invariant.CanConj.VariadicPreflightUsesAccumulatedState");
    TRACE_ID("invariant.CanConj.VariadicPreflightFalseIfAnyStepFails");
    TRACE_ID("invariant.CanConj.VariadicValuesAdmittedRegardlessOfPosition");
    TRACE_ID("invariant.CanConj.VariadicPreflightPreservesSource");
    TRACE_ID("invariant.CanConj.NoHeapAllocation");
    TRACE_ID("invariant.CanConj.NoRtti");
    TRACE_ID("invariant.CanConj.NoExceptions");
    TRACE_ID("invariant.CanConj.SingleThreadedExecutionModel");
    TRACE_ID("invariant.CanConj.ReferentialTransparency");
    TRACE_ID("invariant.CanConj.AppliesToAllConjableCollections");
    TRACE_ID("invariant.CanConj.SetOrMapExistingElementSucceeds");
    TRACE_ID("invariant.CanConj.VectorOrQueueIsCapacityOnly");
    TRACE_ID("invariant.CanConj.StringRequiresCapacityAndValidCharacter");
    TRACE_ID("invariant.CanConj.StringInvalidCharacterReturnsFalse");

    constexpr Vector<int, 4> v_empty{};
    STATIC_REQUIRE(can_conj(v_empty));
    STATIC_REQUIRE(v_empty.count() == 0);

    // Vector and Queue: capacity only, the value does not matter.
    constexpr Queue<int, 4> q0{};
    STATIC_REQUIRE(can_conj(q0, 1));
    constexpr Vector<int, 4> v0{};
    STATIC_REQUIRE(can_conj(v0, 1));
    constexpr cljonic::String<4> text0{"AB"};
    STATIC_REQUIRE(can_conj(text0, 'C'));
    STATIC_REQUIRE_FALSE(can_conj(text0, '\0'));
    STATIC_REQUIRE(can_conj(Vector<int, 2>{}, 1, 2));
    STATIC_REQUIRE_FALSE(can_conj(Vector<int, 2>{1}, 2, 3));
    STATIC_REQUIRE_FALSE(can_conj(Queue<int, 1>{}, 1, 2));
    STATIC_REQUIRE(can_conj(cljonic::String<3>{"a"}, 'b', 'c'));
    STATIC_REQUIRE_FALSE(can_conj(cljonic::String<2>{"a"}, 'b', 'c'));
    STATIC_REQUIRE_FALSE(can_conj(cljonic::String<3>{"a"}, 'b', '\0'));

    // Set and Map: an existing element or key succeeds even when full.
    constexpr Set<int, 4> s0{};
    STATIC_REQUIRE(can_conj(s0, 1));
    constexpr auto m0 = assoc(Map<int, int, 4>{}, 1, 100);
    STATIC_REQUIRE(can_conj(m0, MapEntry<int, int>{1, 999})); // existing key
    STATIC_REQUIRE(can_conj(m0, MapEntry<int, int>{2, 200})); // new key, room
    STATIC_REQUIRE(can_conj(Set<int, 2>{1}, 1, 2, 1));
    STATIC_REQUIRE_FALSE(can_conj(Set<int, 1>{1}, 1, 2));
    STATIC_REQUIRE(can_conj(Map<int, int, 2>{}, MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}));
    STATIC_REQUIRE_FALSE(
        can_conj(Map<int, int, 1>{MapEntry<int, int>{1, 10}}, MapEntry<int, int>{1, 20}, MapEntry<int, int>{2, 30}));

    // Full collections: Vector/Queue capacity-bound; Set/Map duplicate still true.
    constexpr auto q_full = conj(Queue<int, 1>{}, 1);
    STATIC_REQUIRE_FALSE(can_conj(q_full, 2));
    constexpr Vector<int, 1> v_full{1};
    STATIC_REQUIRE_FALSE(can_conj(v_full, 2));
    constexpr cljonic::String<1> text_full{"A"};
    STATIC_REQUIRE_FALSE(can_conj(text_full, 'B'));
    STATIC_REQUIRE_FALSE(can_conj(text_full, '\0'));
    constexpr auto s_full = conj(Set<int, 1>{}, 1);
    STATIC_REQUIRE(can_conj(s_full, 1));
    STATIC_REQUIRE_FALSE(can_conj(s_full, 2));
    constexpr auto m_full = assoc(Map<int, int, 1>{}, 1, 100);
    STATIC_REQUIRE(can_conj(m_full, MapEntry<int, int>{1, 999}));
    STATIC_REQUIRE_FALSE(can_conj(m_full, MapEntry<int, int>{2, 200}));

    // Runtime tests for code coverage instrumentation
    volatile int v_raw = 10;
    int v = v_raw;
    auto rq = Queue<int, 4>{};
    REQUIRE(can_conj(rq, v));
    REQUIRE(can_conj(rq));
    REQUIRE(can_conj(rq, 1, 2));
    auto rs = Set<int, 4>{};
    REQUIRE(can_conj(rs, v));
    REQUIRE_FALSE(can_conj(Queue<int, 1>{1}, 2, 3));
}
