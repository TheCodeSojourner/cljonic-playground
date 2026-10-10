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
    using cljonic::String;
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
    TRACE_ID("invariant.Conj.SupportsStringConj");
    TRACE_ID("invariant.Conj.StringConjAppendsAtLogicalCount");
    TRACE_ID("invariant.Conj.FullStringConjReturnsUnchangedCopy");
    TRACE_ID("invariant.Conj.StringConjUsesStringCharacterPolicy");
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

    constexpr String<4> text0{"AB"};
    constexpr auto text1 = conj(text0, '!');
    STATIC_REQUIRE(text1.count() == 3U);
    STATIC_REQUIRE(text1(2) == '!');
    STATIC_REQUIRE(text0.count() == 2U);

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
    constexpr String<2> text_full{"AB"};
    STATIC_REQUIRE(conj(text_full, '!').count() == 2U);
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

TEST_CASE("Conj zero-value and variadic arities", "[conj]") {
    using cljonic::assoc;
    using cljonic::conj;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::peek;
    using cljonic::Queue;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("invariant.Conj.SupportsZeroValueConj");
    TRACE_ID("invariant.Conj.ZeroValueConjReturnsUnchangedCopy");
    TRACE_ID("invariant.Conj.SupportsVariadicConj");
    TRACE_ID("invariant.Conj.VariadicFoldsValuesLeftToRight");
    TRACE_ID("invariant.Conj.VariadicRequiresAtLeastTwoValues");
    TRACE_ID("invariant.Conj.VariadicValueAdmittedUnderCollectionDomain");
    TRACE_ID("invariant.Conj.VariadicOutOfDomainValueRejectedAtCompileTime");
    TRACE_ID("invariant.Conj.VariadicOverCapacityValueIsNoOpAndContinues");

    // Zero-value arity: every supported collection returns an unchanged copy.
    constexpr Vector<int, 4> v0{10, 20};
    constexpr auto v_identity = conj(v0);
    STATIC_REQUIRE(v_identity.count() == 2U);
    STATIC_REQUIRE(v_identity(0) == 10);
    STATIC_REQUIRE(v_identity(1) == 20);

    constexpr auto q_identity = conj(Queue<int, 4>{});
    STATIC_REQUIRE(q_identity.count() == 0U);

    constexpr auto s_identity = conj(Set<int, 4>{1, 2});
    STATIC_REQUIRE(s_identity.count() == 2U);

    constexpr auto text_identity = conj(String<4>{"AB"});
    STATIC_REQUIRE(text_identity.count() == 2U);

    constexpr auto m0 = assoc(Map<int, int, 4>{}, 1, 100);
    constexpr auto m_identity = conj(m0);
    STATIC_REQUIRE(m_identity.count() == 1U);
    STATIC_REQUIRE(m_identity(1) == 100);

    // Variadic arity: values fold left to right, equivalent to nested calls.
    constexpr auto v_fold = conj(Vector<int, 4>{10}, 20, 30, 40);
    STATIC_REQUIRE(v_fold.count() == 4U);
    STATIC_REQUIRE(v_fold(0) == 10);
    STATIC_REQUIRE(v_fold(1) == 20);
    STATIC_REQUIRE(v_fold(2) == 30);
    STATIC_REQUIRE(v_fold(3) == 40);
    constexpr auto v_equivalent = conj(conj(conj(Vector<int, 4>{10}, 20), 30), 40);
    STATIC_REQUIRE(v_fold(3) == v_equivalent(3));

    constexpr auto q_fold = conj(Queue<int, 4>{}, 1, 2, 3);
    STATIC_REQUIRE(q_fold.count() == 3U);
    STATIC_REQUIRE(peek(q_fold) == 1);

    constexpr auto s_fold = conj(Set<int, 4>{1}, 2, 3);
    STATIC_REQUIRE(s_fold.count() == 3U);
    STATIC_REQUIRE(s_fold.contains(3));

    // A duplicate element is a successful no-op inside the fold.
    constexpr auto s_duplicate_fold = conj(Set<int, 4>{1, 2}, 2, 3);
    STATIC_REQUIRE(s_duplicate_fold.count() == 3U);

    constexpr auto text_fold = conj(String<4>{"A"}, 'B', 'C');
    STATIC_REQUIRE(text_fold.count() == 3U);
    STATIC_REQUIRE(text_fold(1) == 'B');
    STATIC_REQUIRE(text_fold(2) == 'C');

    constexpr auto m_fold = conj(Map<int, int, 4>{}, MapEntry<int, int>{1, 100}, MapEntry<int, int>{2, 200});
    STATIC_REQUIRE(m_fold.count() == 2U);
    STATIC_REQUIRE(m_fold(1) == 100);
    STATIC_REQUIRE(m_fold(2) == 200);

    // An over-capacity value is a per-value no-op that does not stop later values.
    constexpr auto m_full = assoc(Map<int, int, 1>{}, 1, 100);
    constexpr auto m_continued = conj(m_full, MapEntry<int, int>{2, 200}, MapEntry<int, int>{1, 999});
    STATIC_REQUIRE(m_continued.count() == 1U);
    STATIC_REQUIRE(m_continued(1) == 999);
    STATIC_REQUIRE_FALSE(m_continued.contains(2));

    constexpr auto v_full_fold = conj(Vector<int, 2>{1}, 2, 3);
    STATIC_REQUIRE(v_full_fold.count() == 2U);

    constexpr auto text_full_fold = conj(String<2>{"A"}, 'B', 'C');
    STATIC_REQUIRE(text_full_fold.count() == 2U);

    // Runtime coverage paths for the zero-value and variadic overloads.
    volatile int rv_raw = 7;
    int rv = rv_raw;
    auto runtime_vector = Vector<int, 4>{rv};
    auto runtime_vector_identity = conj(runtime_vector);
    REQUIRE(runtime_vector_identity.count() == 1U);

    volatile int rq1_raw = 1;
    volatile int rq2_raw = 2;
    int rq1_value = rq1_raw;
    int rq2_value = rq2_raw;
    auto runtime_queue = Queue<int, 4>{};
    auto runtime_queue_fold = conj(runtime_queue, rq1_value, rq2_value);
    REQUIRE(runtime_queue_fold.count() == 2U);

    auto runtime_set = Set<int, 4>{};
    auto runtime_set_fold = conj(runtime_set, rq1_value, rq2_value);
    REQUIRE(runtime_set_fold.count() == 2U);

    auto runtime_map = Map<int, int, 4>{};
    auto runtime_map_fold = conj(runtime_map, MapEntry<int, int>{rq1_value, rq2_value}, MapEntry<int, int>{2, 200});
    REQUIRE(runtime_map_fold.count() == 2U);

    auto runtime_string = String<4>{"A"};
    auto runtime_string_fold = conj(runtime_string, 'B', 'C');
    REQUIRE(runtime_string_fold.view() == "ABC");

    volatile char invalid_char_raw = static_cast<char>(0x80);
    char invalid_char = invalid_char_raw;
    auto runtime_string_invalid = conj(runtime_string, 'B', invalid_char);
    REQUIRE(runtime_string_invalid.view() == "AB.");
}
