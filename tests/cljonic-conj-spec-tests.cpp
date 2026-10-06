#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Conj free function operations", "[conj]") {
    using cljonic::assoc;
    using cljonic::conj;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::Set;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Conj");
    TRACE_ID("invariant.Conj.ConjFreeFunctionSupported");
    TRACE_ID("invariant.Conj.NoHeapAllocation");
    TRACE_ID("invariant.Conj.NoRtti");
    TRACE_ID("invariant.Conj.NoExceptions");
    TRACE_ID("invariant.Conj.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Conj.FunctionalImmutabilityPreserved");
    TRACE_ID("invariant.Conj.ReferentialTransparency");
    TRACE_ID("invariant.Conj.CopyOnModifySemantics");
    TRACE_ID("invariant.Conj.SupportsVectorConj");
    TRACE_ID("invariant.Conj.SupportsSetConj");
    TRACE_ID("invariant.Conj.SupportsMapConj");
    TRACE_ID("invariant.Conj.SupportsQueueConj");
    TRACE_ID("invariant.Conj.ReplacesDuplicateKeyOrElement");

    constexpr Queue<int, 4> q0{};
    constexpr auto q1 = conj(q0, 10);
    STATIC_REQUIRE(q1.count() == 1U);
    STATIC_REQUIRE(q0.count() == 0U);

    constexpr Set<int, 4> s0{};
    constexpr auto s1 = conj(s0, 1);
    STATIC_REQUIRE(s1.count() == 1U);
    STATIC_REQUIRE(s0.count() == 0U);

    // Vector: conj appends at the end (highest logical index).
    constexpr Vector<int, 4> v0{10, 20};
    constexpr auto v1 = conj(v0, 30);
    STATIC_REQUIRE(v1.count() == 3U);
    STATIC_REQUIRE(v1(2) == 30);
    STATIC_REQUIRE(v1(0) == 10);
    STATIC_REQUIRE(v0.count() == 2U);

    // Map: conj associates one entry; an existing key replaces its value.
    constexpr auto m0 = assoc(Map<int, int, 4>{}, 1, 100);
    constexpr auto m_dup = conj(m0, MapEntry<int, int>{1, 999});
    STATIC_REQUIRE(m_dup.count() == 1U);
    STATIC_REQUIRE(m_dup(1) == 999);
    constexpr auto m_new = conj(m0, MapEntry<int, int>{2, 200});
    STATIC_REQUIRE(m_new.count() == 2U);
    STATIC_REQUIRE(m_new(2) == 200);

    // Full collections return unchanged copies.
    constexpr Vector<int, 2> v_full{1, 2};
    STATIC_REQUIRE(conj(v_full, 3).count() == 2U);
    constexpr auto m_full = assoc(Map<int, int, 1>{}, 1, 100);
    STATIC_REQUIRE(conj(m_full, MapEntry<int, int>{2, 200}).count() == 1U);
    STATIC_REQUIRE(conj(m_full, MapEntry<int, int>{1, 999})(1) == 999);

    // Runtime tests for code coverage instrumentation
    volatile int qv_raw = 55;
    volatile int sv_raw = 66;
    int qv = qv_raw;
    int sv = sv_raw;
    auto rq = Queue<int, 4>{};
    auto rq1 = conj(rq, qv);
    REQUIRE(rq1.count() == 1U);
    REQUIRE(rq.count() == 0U);

    auto rs = Set<int, 4>{};
    auto rs1 = conj(rs, sv);
    REQUIRE(rs1.count() == 1U);
    REQUIRE(rs.count() == 0U);
}
