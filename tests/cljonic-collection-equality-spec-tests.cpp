#include <catch2/catch_test_macros.hpp>

#include <string>
#include <variant>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

// ============================================================================
// Runtime traversal coverage for collection begin()/end()
// ============================================================================

TEST_CASE("Collection begin/end runtime traversal", "[equality][traversal]") {
    // Map
    const auto m = cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 10}, cljonic::MapEntry<int, int>{2, 20}};
    auto map_count = 0U;
    for (auto it = m.begin(); it != m.end(); ++it) {
        map_count += (it->value == 10 || it->value == 20) ? 1U : 0U;
    }
    CHECK(map_count == 2U);

    // Set
    const auto s = cljonic::Set<int, 4>{3, 1, 2};
    auto set_count = 0U;
    for (auto it = s.begin(); it != s.end(); ++it) {
        set_count += (*it > 0) ? 1U : 0U;
    }
    CHECK(set_count == 3U);

    // String
    const auto str = cljonic::String<8>{"hi"};
    auto str_count = 0U;
    for (auto it = str.begin(); it != str.end(); ++it) {
        str_count += (*it == 'h' || *it == 'i') ? 1U : 0U;
    }
    CHECK(str_count == 2U);
}

// ============================================================================
// Collection equality (REQ-COLL-021)
// ============================================================================

TEST_CASE("Vector equality is order-sensitive", "[equality][vector]") {
    TRACE_ID("entity-fields.CollectionEquality");
    TRACE_ID("invariant.CollectionEquality.OrderSensitiveForVectorStringQueue");
    TRACE_ID("invariant.CollectionEquality.ConstexprNoexceptNonmutatingNonallocating");
    TRACE_ID("entity-fields.VectorEquality");
    TRACE_ID("invariant.VectorEquality.OrderSensitive");
    TRACE_ID("invariant.VectorEquality.ComparesCountAndElementsInLogicalOrder");

    constexpr cljonic::Vector<int, 4> a{1, 2, 3};
    constexpr cljonic::Vector<int, 4> b{1, 2, 3};
    constexpr cljonic::Vector<int, 4> c{1, 3, 2};
    constexpr cljonic::Vector<int, 4> d{1, 2};

    STATIC_REQUIRE(a == b);
    STATIC_REQUIRE(!(a == c));
    STATIC_REQUIRE(!(a == d));
    STATIC_REQUIRE(a == a);

    const auto ra = cljonic::Vector<int, 4>{1, 2, 3};
    const auto rb = cljonic::Vector<int, 4>{1, 2, 3};
    const auto rc = cljonic::Vector<int, 4>{3, 2, 1};
    const auto rd = cljonic::Vector<int, 4>{1, 2};
    CHECK(ra == rb);
    CHECK_FALSE(ra == rc);
    CHECK_FALSE(ra == rd);                        // count mismatch
    CHECK_FALSE(cljonic::Vector<int, 4>{} == rb); // empty vs non-empty
}

TEST_CASE("Queue equality is order-sensitive in FIFO order", "[equality][queue]") {
    TRACE_ID("invariant.CollectionEquality.OrderSensitiveForVectorStringQueue");
    TRACE_ID("entity-fields.QueueEquality");
    TRACE_ID("invariant.QueueEquality.OrderSensitive");
    TRACE_ID("invariant.QueueEquality.ComparesCountAndElementsInFifoOrder");

    constexpr cljonic::Queue<int, 4> a{1, 2, 3};
    constexpr cljonic::Queue<int, 4> b{1, 2, 3};
    constexpr cljonic::Queue<int, 4> c{3, 2, 1};

    STATIC_REQUIRE(a == b);
    STATIC_REQUIRE(!(a == c));
    STATIC_REQUIRE(a == a);

    const auto ra = cljonic::Queue<int, 4>{1, 2, 3};
    const auto rb = cljonic::Queue<int, 4>{1, 2, 3};
    CHECK(ra == rb);

    // FIFO order is preserved after pushes and pops.
    auto q = cljonic::Queue<int, 4>{};
    q = q.conj(1).conj(2).conj(3);
    q = q.pop();
    q = q.conj(4);
    const auto expected = cljonic::Queue<int, 4>{2, 3, 4};
    CHECK(q == expected);

    // Runtime unequal queues: element mismatch and count mismatch.
    CHECK_FALSE(q == cljonic::Queue<int, 4>{2, 3, 5});
    CHECK_FALSE(q == cljonic::Queue<int, 4>{2, 3});
    CHECK_FALSE(q == cljonic::Queue<int, 4>{});
}

TEST_CASE("String equality is order-sensitive in content order", "[equality][string]") {
    TRACE_ID("invariant.CollectionEquality.OrderSensitiveForVectorStringQueue");
    TRACE_ID("entity-fields.StringEquality");
    TRACE_ID("invariant.StringEquality.OrderSensitive");
    TRACE_ID("invariant.StringEquality.ComparesCountAndContentInLogicalOrder");

    constexpr cljonic::String<8> a{"hello"};
    constexpr cljonic::String<8> b{"hello"};
    constexpr cljonic::String<8> c{"olleh"};
    constexpr cljonic::String<8> d{"hell"};

    STATIC_REQUIRE(a == b);
    STATIC_REQUIRE(!(a == c));
    STATIC_REQUIRE(!(a == d));
    STATIC_REQUIRE(a == a);

    // Runtime unequal strings: content mismatch and count mismatch.
    const auto rs = cljonic::String<8>{"hello"};
    CHECK(rs == cljonic::String<8>{"hello"});
    CHECK_FALSE(rs == cljonic::String<8>{"world"});
    CHECK_FALSE(rs == cljonic::String<8>{"hell"});
    CHECK_FALSE(rs == cljonic::String<8>{""});
}

TEST_CASE("Map equality is order-insensitive", "[equality][map]") {
    TRACE_ID("invariant.CollectionEquality.OrderInsensitiveForMapSet");
    TRACE_ID("entity-fields.MapEquality");
    TRACE_ID("invariant.MapEquality.OrderInsensitive");
    TRACE_ID("invariant.MapEquality.ComparesCountAndKeyValueMatchingWithoutOrder");
    TRACE_ID("invariant.MapEquality.UsesStableKeyAndValueEquality");

    constexpr cljonic::Map<int, int, 4> a{cljonic::MapEntry<int, int>{1, 10}, cljonic::MapEntry<int, int>{2, 20}};
    constexpr cljonic::Map<int, int, 4> b{cljonic::MapEntry<int, int>{2, 20}, cljonic::MapEntry<int, int>{1, 10}};
    constexpr cljonic::Map<int, int, 4> c{cljonic::MapEntry<int, int>{1, 10}, cljonic::MapEntry<int, int>{2, 30}};

    STATIC_REQUIRE(a == b);    // different insertion order, same pairs
    STATIC_REQUIRE(!(a == c)); // value differs for key 2
    STATIC_REQUIRE(a == a);

    // Runtime check with the member assoc path.
    auto m1 = cljonic::Map<int, int, 4>{};
    m1 = m1.assoc(1, 100);
    m1 = m1.assoc(2, 200);
    auto m2 = cljonic::Map<int, int, 4>{};
    m2 = m2.assoc(2, 200);
    m2 = m2.assoc(1, 100);
    CHECK(m1 == m2);

    // Runtime unequal maps: value mismatch for a shared key.
    auto m3 = cljonic::Map<int, int, 4>{};
    m3 = m3.assoc(1, 100);
    m3 = m3.assoc(2, 999);
    CHECK_FALSE(m1 == m3);

    // Runtime unequal maps: same count, disjoint keys (missing-key path).
    auto m4 = cljonic::Map<int, int, 4>{};
    m4 = m4.assoc(7, 100);
    m4 = m4.assoc(8, 200);
    CHECK_FALSE(m1 == m4);
    CHECK_FALSE(m4 == m1);

    // Runtime count mismatch.
    auto m5 = cljonic::Map<int, int, 4>{};
    m5 = m5.assoc(1, 100);
    CHECK_FALSE(m1 == m5);
    CHECK_FALSE(cljonic::Map<int, int, 4>{} == m1);
}

TEST_CASE("Set equality is order-insensitive", "[equality][set]") {
    TRACE_ID("invariant.CollectionEquality.OrderInsensitiveForMapSet");
    TRACE_ID("entity-fields.SetEquality");
    TRACE_ID("invariant.SetEquality.OrderInsensitive");
    TRACE_ID("invariant.SetEquality.ComparesCountAndElementMatchingWithoutOrder");
    TRACE_ID("invariant.SetEquality.UsesStableElementEquality");

    constexpr cljonic::Set<int, 4> a{3, 1, 2};
    constexpr cljonic::Set<int, 4> b{1, 2, 3};
    constexpr cljonic::Set<int, 4> c{1, 2, 4};

    STATIC_REQUIRE(a == b);
    STATIC_REQUIRE(!(a == c));
    STATIC_REQUIRE(a == a);

    // Runtime unequal sets: element mismatch and count mismatch.
    const auto ra = cljonic::Set<int, 4>{1, 2, 3};
    const auto rb = cljonic::Set<int, 4>{1, 2, 4};
    const auto rc = cljonic::Set<int, 4>{1, 2};
    CHECK(ra == cljonic::Set<int, 4>{3, 2, 1});
    CHECK_FALSE(ra == rb);
    CHECK_FALSE(ra == rc);
    CHECK_FALSE(cljonic::Set<int, 4>{} == ra);
}

TEST_CASE("Collection equality requires stable-equality components", "[equality][concepts]") {
    TRACE_ID("invariant.CollectionEquality.RequiresStableEqualityComponents");

    using namespace cljonic::concepts;

    // Collections with float-bearing components are storable (REQ-NUM-001) but
    // are not stable-equality comparable and therefore provide no operator==.
    STATIC_REQUIRE(NothrowCollectionElement<double>);
    STATIC_REQUIRE(StableEqualityComparable<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Vector<double, 4>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Map<int, double, 4>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::MapEntry<int, double>>);

    // Float-bearing map values / vector elements still compile (REQ-NUM-001).
    constexpr cljonic::Vector<double, 4> vd{1.0, 2.0};
    STATIC_REQUIRE(vd.count() == 2);
    constexpr cljonic::Map<int, double, 4> md{cljonic::MapEntry<int, double>{1, 1.5}};
    STATIC_REQUIRE(md.count() == 1);
}

TEST_CASE("Recursive collection equality over variant and nested collections", "[equality][composite]") {
    TRACE_ID("invariant.CollectionEquality.RecursionIntoNestedValues");
    TRACE_ID("invariant.CollectionEquality.BoundedTraversalOnly");
    TRACE_ID("invariant.CollectionEquality.NoImplicitSequenceEqualityForProducers");
    TRACE_ID("invariant.CollectionEquality.ProducerComponentsUseProducerParameterEquality");
    TRACE_ID("invariant.CollectionEquality.NoCrossTypeNumericUnification");

    using vec = cljonic::Vector<int, 4>;
    using entry = cljonic::MapEntry<vec, int>;

    constexpr vec inner1{1, 2, 3};
    constexpr vec inner2{4, 5, 6};
    constexpr vec inner3{1, 2, 4};

    constexpr cljonic::Vector<vec, 4> outer1{inner1, inner2};
    constexpr cljonic::Vector<vec, 4> outer2{inner2, inner1};
    constexpr cljonic::Vector<vec, 4> outer3{inner3, inner1};

    STATIC_REQUIRE(outer1 == outer1);
    STATIC_REQUIRE(!(outer1 == outer2)); // order-sensitive: order differs
    STATIC_REQUIRE(!(outer1 == outer3));

    constexpr cljonic::Map<vec, int, 4> m1{entry{inner1, 10}, entry{inner2, 20}};
    constexpr cljonic::Map<vec, int, 4> m2{entry{inner2, 20}, entry{inner1, 10}};
    STATIC_REQUIRE(m1 == m2); // order-insensitive even over nested keys

    // Alternative-strict variant keys: same alternative equal, different not.
    using alt = std::variant<int, long>;
    constexpr cljonic::Map<alt, int, 4> vm{cljonic::MapEntry<alt, int>{alt{1}, 10}};
    STATIC_REQUIRE(vm(alt{1}) == 10);
    STATIC_REQUIRE(vm(alt{1L}, -1) == -1); // NoCrossTypeNumericUnification

    // ProducerComponentsUseProducerParameterEquality: nested producer values
    // compare by producer parameter equality (never by materialized
    // sequence), so a bounded collection containing producers acquires
    // equality exactly when the producer stored parameters admit stable
    // equality.
    constexpr cljonic::Vector<cljonic::Range<int>, 4> producer_vec_a{cljonic::Range<int>{1, 5, 2},
                                                                     cljonic::Range<int>{2, 4}};
    constexpr cljonic::Vector<cljonic::Range<int>, 4> producer_vec_b{cljonic::Range<int>{1, 5, 2},
                                                                     cljonic::Range<int>{2, 4}};
    constexpr cljonic::Vector<cljonic::Range<int>, 4> producer_vec_c{cljonic::Range<int>{1, 5, 2},
                                                                     cljonic::Range<int>{2, 5}};
    STATIC_REQUIRE(producer_vec_a == producer_vec_b);
    STATIC_REQUIRE(!(producer_vec_a == producer_vec_c));

    // BoundedTraversalOnly: equality over complete values, never over
    // unbounded sequences; callable-bearing producers are rejected for
    // producer parameter equality at the type boundary and are verified by the
    // variant-compile-fail gate.
}