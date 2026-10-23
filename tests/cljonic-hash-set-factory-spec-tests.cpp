#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <type_traits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("hash_set factory builds bounded sets from individual element arguments", "[set][factory]") {
    using cljonic::hash_set;
    using cljonic::Set;

    TRACE_ID("entity-fields.HashSetFactory");
    TRACE_ID("invariant.HashSetFactory.SupportsDeducedElementFactory");
    TRACE_ID("invariant.HashSetFactory.DeducedElementTypeIsUnqualifiedFirstArgument");
    TRACE_ID("invariant.HashSetFactory.SupportsExplicitElementTypeFactory");
    TRACE_ID("invariant.HashSetFactory.SupportsExplicitCapacityFactory");
    TRACE_ID("invariant.HashSetFactory.DeducedCapacityEqualsArgumentCount");
    TRACE_ID("invariant.HashSetFactory.ExplicitCapacityDeterminesResultCapacity");
    TRACE_ID("invariant.HashSetFactory.ExplicitCapacityFactorySupportsEmptyArguments");
    TRACE_ID("invariant.HashSetFactory.ExplicitCapacityEmptyFactorySupportsZeroCapacity");
    TRACE_ID("invariant.HashSetFactory.ExplicitCapacityIsAtLeastArgumentCount");
    TRACE_ID("invariant.HashSetFactory.ExplicitCapacityIsBoundedByConfiguredMaximum");
    TRACE_ID("invariant.HashSetFactory.NonemptyFormsRequireAtLeastOneArgument");
    TRACE_ID("invariant.HashSetFactory.EmptyFactoryRequiresExplicitElementTypeAndCapacity");
    TRACE_ID("invariant.HashSetFactory.EmptyFactoryWithoutExplicitCapacityIsCompileTimeRejected");
    TRACE_ID("invariant.HashSetFactory.UntypedEmptyCallIsCompileTimeRejected");
    TRACE_ID("invariant.HashSetFactory.ArgumentCountIsBoundedByConfiguredMaximum");
    TRACE_ID("invariant.HashSetFactory.OversizedArgumentCountIsCompileTimeRejected");
    TRACE_ID("invariant.HashSetFactory.ElementTypeSatisfiesSetElementAdmission");
    TRACE_ID("invariant.HashSetFactory.EveryArgumentConstructsSafelyWithoutThrowing");
    TRACE_ID("invariant.HashSetFactory.RuntimeDuplicateConstructionDeduplicates");
    TRACE_ID("invariant.HashSetFactory.ConstantEvaluatedDuplicateConstructionIsRejected");
    TRACE_ID("invariant.HashSetFactory.DeducedCapacityIsArgumentCountDespiteDeduplication");
    TRACE_ID("invariant.HashSetFactory.PreservesPublicSetTypeAndExistingConstructors");
    TRACE_ID("invariant.HashSetFactory.ReturnsIndependentlyOwningSet");
    TRACE_ID("invariant.HashSetFactory.ConstexprNoexceptNonallocating");
    TRACE_ID("invariant.HashSetFactory.NoRtti");
    TRACE_ID("invariant.HashSetFactory.NoExceptions");
    TRACE_ID("invariant.HashSetFactory.ReferentialTransparency");

    constexpr auto deduced = hash_set(1, 2, short{3});
    constexpr auto explicit_types = hash_set<long>(short{1}, short{2});
    constexpr auto explicit_capacity = hash_set<int, 4>(1, 2);
    constexpr auto empty = hash_set<int, 0>();
    constexpr auto reserved_empty = hash_set<int, 4>();

    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(deduced)>, Set<int, 3>>);
    STATIC_REQUIRE(deduced.capacity() == 3U);
    STATIC_REQUIRE(deduced.count() == 3U);
    STATIC_REQUIRE(deduced(1) == 1);
    STATIC_REQUIRE(deduced(2) == 2);
    STATIC_REQUIRE(deduced(3) == 3);
    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(explicit_types)>, Set<long, 2>>);
    STATIC_REQUIRE(explicit_types(1L) == 1L);
    STATIC_REQUIRE(explicit_types(2L) == 2L);
    STATIC_REQUIRE(explicit_capacity.capacity() == 4U);
    STATIC_REQUIRE(explicit_capacity.count() == 2U);
    STATIC_REQUIRE(empty.capacity() == 0U);
    STATIC_REQUIRE(empty.count() == 0U);
    STATIC_REQUIRE(reserved_empty.capacity() == 4U);
    STATIC_REQUIRE(reserved_empty.count() == 0U);
    STATIC_REQUIRE(noexcept(hash_set(1, 2)));

    const auto runtime_dedup_value = hash_set(1, 2, 2);
    REQUIRE(runtime_dedup_value.capacity() == 3U);
    REQUIRE(runtime_dedup_value.count() == 2U);
    REQUIRE(runtime_dedup_value(2) == 2);

    volatile int runtime_value_raw = 7;
    volatile short runtime_other_raw = 70;
    const int runtime_value = runtime_value_raw;
    const short runtime_other = runtime_other_raw;
    const auto runtime = hash_set(runtime_value, runtime_other);
    REQUIRE(runtime.capacity() == 2U);
    REQUIRE(runtime.count() == 2U);
    REQUIRE(runtime(runtime_value) == 7);
    REQUIRE(runtime(70) == 70);
    const auto runtime_dedup_runtime = hash_set(runtime_value, runtime_value);
    REQUIRE(runtime_dedup_runtime.capacity() == 2U);
    REQUIRE(runtime_dedup_runtime.count() == 1U);
    REQUIRE(runtime_dedup_runtime(7) == 7);
    REQUIRE(deduced(3) == 3);
}
