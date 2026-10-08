#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {
struct DissocNotConvertible {};
} // namespace

TEST_CASE("Dissoc free function operations", "[dissoc]") {
    using cljonic::assoc;
    using cljonic::dissoc;
    using cljonic::Map;

    TRACE_ID("entity-fields.Dissoc");
    TRACE_ID("invariant.Dissoc.DissocFreeFunctionSupported");
    TRACE_ID("invariant.Dissoc.NoHeapAllocation");
    TRACE_ID("invariant.Dissoc.NoRtti");
    TRACE_ID("invariant.Dissoc.NoExceptions");
    TRACE_ID("invariant.Dissoc.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Dissoc.FunctionalImmutabilityPreserved");
    TRACE_ID("invariant.Dissoc.ReferentialTransparency");
    TRACE_ID("invariant.Dissoc.CopyOnModifySemantics");
    TRACE_ID("invariant.Dissoc.MapMemberKeyAdmission");
    TRACE_ID("invariant.Dissoc.ZeroOrMoreKeys");
    TRACE_ID("invariant.Dissoc.KeysAreRemovedLeftToRight");
    TRACE_ID("invariant.Dissoc.AbsentOrRepeatedKeysAreNoOps");
    TRACE_ID("invariant.Dissoc.ZeroKeysReturnUnchangedCopy");
    TRACE_ID("invariant.Dissoc.PresentKeyIsRemovedViaSwapAndRemove");
    TRACE_ID("invariant.Dissoc.UnsupportedDomainsAreRejected");

    using MapType = Map<int, int, 4>;
    STATIC_REQUIRE(cljonic::concepts_detail::DissocKeyAdmissible<MapType, int>);
    STATIC_REQUIRE(cljonic::concepts_detail::DissocKeyAdmissible<MapType, short>);
    STATIC_REQUIRE(cljonic::concepts_detail::all_dissoc_keys_admissible_v<MapType, int, short>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::all_dissoc_keys_admissible_v<MapType, int, DissocNotConvertible>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::DissocKeyAdmissible<cljonic::Set<int, 4>, int>);

    constexpr MapType empty_map{};
    STATIC_REQUIRE(cljonic::concepts_detail::all_dissoc_keys_admissible_v<MapType>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::all_dissoc_keys_admissible_v<cljonic::Set<int, 4>>);
    STATIC_REQUIRE(dissoc(empty_map).count() == 0U);

    constexpr auto m0 = Map{cljonic::MapEntry{1, 100}, cljonic::MapEntry{2, 200}, cljonic::MapEntry{3, 300}};
    constexpr auto empty_result = dissoc(m0);
    STATIC_REQUIRE(empty_result.count() == m0.count());
    STATIC_REQUIRE(empty_result.contains(1));
    STATIC_REQUIRE(empty_result.contains(2));
    STATIC_REQUIRE(empty_result.contains(3));

    constexpr auto one_key = dissoc(m0, 2);
    STATIC_REQUIRE(one_key.count() == 2U);
    STATIC_REQUIRE_FALSE(one_key.contains(2));
    STATIC_REQUIRE(one_key.contains(1));
    STATIC_REQUIRE(one_key.contains(3));

    constexpr auto multiple_keys = dissoc(m0, short{1}, 3, 1);
    STATIC_REQUIRE(multiple_keys.count() == 1U);
    STATIC_REQUIRE_FALSE(multiple_keys.contains(1));
    STATIC_REQUIRE_FALSE(multiple_keys.contains(3));
    STATIC_REQUIRE(multiple_keys.contains(2));
    STATIC_REQUIRE(m0.count() == 3U);
    STATIC_REQUIRE(m0.contains(1));
    STATIC_REQUIRE(m0.contains(2));
    STATIC_REQUIRE(m0.contains(3));

    constexpr auto absent_key = dissoc(m0, 9);
    STATIC_REQUIRE(absent_key.count() == m0.count());
    STATIC_REQUIRE(absent_key.contains(1));
    STATIC_REQUIRE(absent_key.contains(2));
    STATIC_REQUIRE(absent_key.contains(3));

    // Runtime tests for code coverage instrumentation
    volatile int k_raw = 10;
    int k = k_raw;
    auto rm = MapType{};
    auto rm1 = assoc(rm, k, 100);
    auto rm2 = dissoc(rm1, k);
    REQUIRE_FALSE(rm2.contains(k));
    REQUIRE(rm1.contains(k));
}
