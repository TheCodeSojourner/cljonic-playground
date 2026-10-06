#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

struct NotConvertible {};

} // namespace

TEST_CASE("Assoc free function operations", "[assoc]") {
    using cljonic::assoc;
    using cljonic::Map;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Assoc");
    TRACE_ID("invariant.Assoc.AssocFreeFunctionSupported");
    TRACE_ID("invariant.Assoc.RequiresAssociativeCollectionSupport");
    TRACE_ID("invariant.Assoc.SupportsMapAssoc");
    TRACE_ID("invariant.Assoc.SupportsVectorAssoc");
    TRACE_ID("invariant.Assoc.SupportsStringAssoc");
    TRACE_ID("invariant.Assoc.SupportsVariadicAssoc");
    TRACE_ID("invariant.Assoc.VariadicFoldsPairsLeftToRight");
    TRACE_ID("invariant.Assoc.VariadicRequiresAtLeastTwoPairs");
    TRACE_ID("invariant.Assoc.VariadicPairAdmittedUnderCollectionDomain");
    TRACE_ID("invariant.Assoc.VariadicOutOfDomainPairRejectedAtCompileTime");
    TRACE_ID("invariant.Assoc.VariadicOverCapacityPairIsNoOpAndContinues");
    TRACE_ID("invariant.Assoc.VariadicOddTrailingArgumentCountRejected");
    TRACE_ID("invariant.Assoc.KeyDomainIsValidated");
    TRACE_ID("invariant.Assoc.SupportsCollectionSpecificAppendPolicy");
    TRACE_ID("invariant.Assoc.SupportsReplaceOrAppendPolicy");
    TRACE_ID("invariant.Assoc.ReturnsNewCollectionValue");
    TRACE_ID("invariant.Assoc.PreservesSource");
    TRACE_ID("invariant.Assoc.InvalidKeyOrIndexReturnsUnchangedCollection");
    TRACE_ID("invariant.Assoc.CompileTimeInvalidCharacterRejected");
    TRACE_ID("invariant.Assoc.RuntimeInvalidCharacterReplacedWithPeriod");
    TRACE_ID("invariant.Assoc.ConstexprExecutionModel");
    TRACE_ID("invariant.Assoc.NoexceptExecutionModel");
    TRACE_ID("invariant.Assoc.NonMutatingSemantics");
    TRACE_ID("invariant.Assoc.NonAllocatingSemantics");
    TRACE_ID("invariant.Assoc.NoHeapAllocation");
    TRACE_ID("invariant.Assoc.NoRtti");
    TRACE_ID("invariant.Assoc.NoExceptions");
    TRACE_ID("invariant.Assoc.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Assoc.FunctionalImmutabilityPreserved");
    TRACE_ID("invariant.Assoc.ReferentialTransparency");
    TRACE_ID("invariant.Assoc.CopyOnModifySemantics");

    constexpr Map<int, int, 4> m0{};
    constexpr auto m1 = assoc(m0, 1, 100);
    STATIC_REQUIRE(m1.contains(1));
    STATIC_REQUIRE(m1(1) == 100);
    STATIC_REQUIRE(m0.count() == 0U);

    // Runtime tests for code coverage instrumentation
    volatile int k_raw = 42;
    volatile int val_raw = 999;
    int k = k_raw;
    int val = val_raw;
    auto rm0 = Map<int, int, 4>{};
    auto rm1 = assoc(rm0, k, val);
    REQUIRE(rm1.contains(k));
    REQUIRE(rm1(k) == 999);
    REQUIRE(rm0.count() == 0U);

    constexpr Vector<int, 4> v0{10, 20};
    constexpr auto v1 = assoc(v0, 1, 200);
    constexpr auto v2 = assoc(v0, 2, 30);
    constexpr auto v3 = assoc(v0, 4, 40);
    constexpr auto v4 = assoc(v0, -1, 40);
    STATIC_REQUIRE(v1(1) == 200);
    STATIC_REQUIRE(v1.count() == 2U);
    STATIC_REQUIRE(v0(1) == 20);
    STATIC_REQUIRE(v2.count() == 3U);
    STATIC_REQUIRE(v2(2) == 30);
    STATIC_REQUIRE(v3.count() == 2U);
    STATIC_REQUIRE(v4.count() == 2U);

    constexpr String<4> s0{"ab"};
    constexpr auto s1 = assoc(s0, 0U, 'A');
    constexpr auto s2 = assoc(s0, 2U, 'c');
    constexpr auto s3 = assoc(s0, 4U, 'x');
    STATIC_REQUIRE(s1.view() == "Ab");
    STATIC_REQUIRE(s2.view() == "abc");
    STATIC_REQUIRE(s2.count() == 3U);
    STATIC_REQUIRE(s2(3U) == '\0');
    STATIC_REQUIRE(s3.view() == "ab");
    constexpr auto s4 = assoc(s0, -1, 'x');
    STATIC_REQUIRE(s4.view() == "ab");

    volatile char invalid_raw = '\0';
    const char invalid = invalid_raw;
    const auto runtime_string = assoc(s0, 1U, invalid);
    REQUIRE(runtime_string.view() == "a.");

    // Variadic form (REQ-FN-002U): left-to-right fold.
    constexpr auto mv = assoc(Map<int, int, 4>{}, 1, 100, 2, 200, 3, 300);
    STATIC_REQUIRE(count(mv) == 3U);
    STATIC_REQUIRE(mv(3) == 300);
    STATIC_REQUIRE(mv(2) == assoc(assoc(assoc(Map<int, int, 4>{}, 1, 100), 2, 200), 3, 300)(2));

    // A repeated key keeps the last value.
    constexpr auto mdup = assoc(Map<int, int, 4>{}, 1, 100, 1, 999);
    STATIC_REQUIRE(mdup(1) == 999);
    STATIC_REQUIRE(count(mdup) == 1U);

    // A pair that cannot be applied at full capacity is a no-op; later pairs still apply.
    constexpr auto mo = assoc(Map<int, int, 2>{}, 1, 100, 2, 200, 3, 300);
    STATIC_REQUIRE(count(mo) == 2U);
    constexpr auto mo2 = assoc(Map<int, int, 2>{}, 1, 100, 2, 200, 3, 300, 1, 111);
    STATIC_REQUIRE(count(mo2) == 2U);
    STATIC_REQUIRE(mo2(1) == 111);

    // Vector and String variadic forms.
    constexpr auto vv = assoc(Vector<int, 4>{10, 20}, 1, 200, 2, 300, 3, 400);
    STATIC_REQUIRE(count(vv) == 4U);
    STATIC_REQUIRE(vv(3) == 400);
    constexpr auto sv = assoc(String<8>{"ab"}, 2U, 'c', 3U, 'd');
    STATIC_REQUIRE(sv.view() == "abcd");

    // Rejections are detected through the admission concepts, not callability
    // (REQ-DIAG-009 detectability contract): an odd trailing argument, an
    // out-of-domain value, and a non-associative collection are all rejected.
    STATIC_REQUIRE(!cljonic::concepts_detail::all_assoc_pairs_admissible_v<Map<int, int, 4>, int, int, int>);
    STATIC_REQUIRE(!cljonic::concepts_detail::AssocPairAdmissible<Map<int, int, 4>, int, NotConvertible>);
    STATIC_REQUIRE(!cljonic::concepts::AssociativeCollection<Set<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts_detail::all_assoc_pairs_admissible_v<Map<int, int, 4>, int, int>);

    // Runtime coverage for the variadic fold.
    volatile int rk1_raw = 5;
    volatile int rk2_raw = 6;
    const int rk1 = rk1_raw;
    const int rk2 = rk2_raw;
    const auto rm = assoc(Map<int, int, 4>{}, rk1, 500, rk2, 600);
    REQUIRE(rm(rk1) == 500);
    REQUIRE(rm(rk2) == 600);
}
