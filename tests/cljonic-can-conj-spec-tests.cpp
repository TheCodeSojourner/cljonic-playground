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
    TRACE_ID("invariant.CanConj.NoHeapAllocation");
    TRACE_ID("invariant.CanConj.NoRtti");
    TRACE_ID("invariant.CanConj.NoExceptions");
    TRACE_ID("invariant.CanConj.SingleThreadedExecutionModel");
    TRACE_ID("invariant.CanConj.ReferentialTransparency");
    TRACE_ID("invariant.CanConj.AppliesToAllConjableCollections");
    TRACE_ID("invariant.CanConj.SetOrMapExistingElementSucceeds");
    TRACE_ID("invariant.CanConj.VectorOrQueueIsCapacityOnly");

    // Vector and Queue: capacity only, the value does not matter.
    constexpr Queue<int, 4> q0{};
    STATIC_REQUIRE(can_conj(q0, 1));
    constexpr Vector<int, 4> v0{};
    STATIC_REQUIRE(can_conj(v0, 1));

    // Set and Map: an existing element or key succeeds even when full.
    constexpr Set<int, 4> s0{};
    STATIC_REQUIRE(can_conj(s0, 1));
    constexpr auto m0 = assoc(Map<int, int, 4>{}, 1, 100);
    STATIC_REQUIRE(can_conj(m0, MapEntry<int, int>{1, 999})); // existing key
    STATIC_REQUIRE(can_conj(m0, MapEntry<int, int>{2, 200})); // new key, room

    // Full collections: Vector/Queue capacity-bound; Set/Map duplicate still true.
    constexpr auto q_full = conj(Queue<int, 1>{}, 1);
    STATIC_REQUIRE_FALSE(can_conj(q_full, 2));
    constexpr Vector<int, 1> v_full{1};
    STATIC_REQUIRE_FALSE(can_conj(v_full, 2));
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
    auto rs = Set<int, 4>{};
    REQUIRE(can_conj(rs, v));
}
