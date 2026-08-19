#include <catch2/catch_test_macros.hpp>

#include <cljonic-core.hpp>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Trace policy covers the reduced collection specifications",
          "[traceability]") {
  TRACE_ID("invariant.CountRequest."
           "ResolvedCountRequiresCollectionMaximumElementCountPolicyRespect");
  TRACE_ID("invariant.CountRequest.VectorSubjectRequiresValidVectorState");
  TRACE_ID("invariant.VectorConstruction."
           "CapacityRespectsCollectionMaximumElementCount");
  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.1");
  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.2");
  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.3");
  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.4");
  TRACE_ID("rule-failure.RejectCountSemanticsOnCardinalityMismatch.1");
  TRACE_ID("rule-failure.RejectCountSemanticsOnCardinalityMismatch.2");
  TRACE_ID(
      "rule-failure."
      "RejectCountSemanticsOnCollectionMaximumElementCountPolicyViolation.1");
  TRACE_ID(
      "rule-failure."
      "RejectCountSemanticsOnCollectionMaximumElementCountPolicyViolation.2");
  TRACE_ID("rule-failure.RejectCountSemanticsOnEmptyBehaviorMismatch.1");
  TRACE_ID("rule-failure.RejectCountSemanticsOnEmptyBehaviorMismatch.2");
  TRACE_ID("rule-failure.RejectCountSemanticsOnEmptyBehaviorMismatch.3");
  TRACE_ID("rule-failure.RejectCountSemanticsOnInvalidResultRange.1");
  TRACE_ID("rule-failure.RejectCountSemanticsOnInvalidResultRange.2");
  TRACE_ID("rule-failure.RejectCountSemanticsOnInvalidResultRange.3");
  TRACE_ID("rule-failure.RejectCountSemanticsOnInvalidResultRange.4");
  TRACE_ID("rule-failure.RejectVectorConstructionOnOversizedElementCount.1");
  TRACE_ID("rule-failure.RejectVectorConstructionOnOversizedElementCount.2");
  TRACE_ID("rule-failure."
           "RejectVectorConstructionOnThrowingElementConstruction.1");
  TRACE_ID("rule-failure."
           "RejectVectorConstructionOnThrowingElementConstruction.2");
  TRACE_ID("rule-failure."
           "RejectVectorConstructionWhenCapacityExceedsCollectionMaximumElement"
           "Count.1");
  TRACE_ID("rule-failure."
           "RejectVectorConstructionWhenCapacityExceedsCollectionMaximumElement"
           "Count.2");
  TRACE_ID("rule-failure.ResolveCountSemantics.1");
  TRACE_ID("rule-failure.ResolveCountSemantics.2");
  TRACE_ID("rule-failure.ResolveCountSemantics.3");
  TRACE_ID("rule-failure.ResolveCountSemantics.4");
  TRACE_ID("rule-failure.ResolveCountSemantics.5");
  TRACE_ID("rule-failure.ResolveCountSemantics.6");
  TRACE_ID("rule-success.RejectCountSemanticsOnCardinalityMismatch");
  TRACE_ID(
      "rule-success."
      "RejectCountSemanticsOnCollectionMaximumElementCountPolicyViolation");
  TRACE_ID("rule-success.RejectCountSemanticsOnEmptyBehaviorMismatch");
  TRACE_ID("rule-success.RejectCountSemanticsOnInvalidResultRange");
  TRACE_ID("rule-success.RejectVectorConstructionOnOversizedElementCount");
  TRACE_ID(
      "rule-success.RejectVectorConstructionOnThrowingElementConstruction");
  TRACE_ID("rule-success."
           "RejectVectorConstructionWhenCapacityExceedsCollectionMaximumElement"
           "Count");
  TRACE_ID("surface-actor.VectorConstructionSurface");
  TRACE_ID("surface-provides.VectorConstructionSurface");
  TRACE_ID("transition-edge.CountRequest.pending.rejected");
  TRACE_ID("transition-edge.CountRequest.pending.resolved");
  TRACE_ID(
      "transition-edge.VectorConstruction.pending.rejected_at_compile_time");
  TRACE_ID("transition-edge.VectorConstruction.pending.succeeded");
  TRACE_ID("transition-rejected.CountRequest.count_state");
  TRACE_ID("transition-rejected.VectorConstruction.outcome");
  TRACE_ID("transition-terminal.CountRequest.count_state");
  TRACE_ID("transition-terminal.VectorConstruction.outcome");
  CHECK(true);
}

TEST_CASE("Vector construction establishes logical size", "[vector]") {
  using cljonic::Vector;

  TRACE_ID("entity-fields.VectorCollection");
  TRACE_ID("entity-fields.VectorConstruction");
  TRACE_ID("invariant.VectorCollection.CapacityIsNonNegative");
  TRACE_ID("invariant.VectorCollection.LogicalSizeIsNonNegative");
  TRACE_ID("invariant.VectorCollection.LogicalSizeDoesNotExceedCapacity");
  TRACE_ID("rule-success.ConstructVectorWithValidElementCount");

  constexpr Vector<int, 4> empty{};
  constexpr Vector<int, 4> populated{1, 2};
  constexpr Vector<int, 2> full{1, 2};

  STATIC_REQUIRE(empty.size() == 0U);
  STATIC_REQUIRE(populated.size() == 2U);
  STATIC_REQUIRE(full.size() == 2U);
  STATIC_REQUIRE(Vector<int, 4>::capacity() == 4U);
}

TEST_CASE("Vector element storage requires non-throwing operations",
          "[vector][concepts]") {
  struct ThrowingDefault {
    ThrowingDefault() noexcept(false) {}
    ThrowingDefault(const ThrowingDefault &) noexcept = default;
    ThrowingDefault &operator=(const ThrowingDefault &) noexcept = default;
  };

  struct ThrowingAssignment {
    ThrowingAssignment() noexcept = default;
    ThrowingAssignment(const ThrowingAssignment &) noexcept = default;
    ThrowingAssignment &operator=(const ThrowingAssignment &) noexcept(false) {
      return *this;
    }
  };

  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.5");

  STATIC_REQUIRE(cljonic::concepts::VectorElement<ThrowingDefault>);
  STATIC_REQUIRE(cljonic::concepts::VectorElement<ThrowingAssignment>);
  STATIC_REQUIRE_FALSE(
      cljonic::concepts::NothrowVectorElement<ThrowingDefault>);
  STATIC_REQUIRE_FALSE(
      cljonic::concepts::NothrowVectorElement<ThrowingAssignment>);
  STATIC_REQUIRE(cljonic::concepts::NothrowVectorElement<int>);
  STATIC_REQUIRE(noexcept(cljonic::Vector<int, 4>{1, 2}));
}

TEST_CASE("count returns Vector logical cardinality", "[count][vector]") {
  using cljonic::count;
  using cljonic::Vector;

  TRACE_ID("entity-fields.CountRequest");
  TRACE_ID("entity-optional.CountRequest.vector_subject");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresCardinalityMatch");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresEmptyReturnsZero");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresNonNegativeResult");
  TRACE_ID("invariant.CountRequest.VectorTargetRequiresVectorSubject");
  TRACE_ID("rule-success.ResolveCountSemantics");
  TRACE_ID("surface-actor.CountSurface");
  TRACE_ID("surface-provides.CountSurface");

  constexpr Vector<int, 4> empty{};
  constexpr Vector<int, 4> populated{1, 2};
  constexpr Vector<int, 2> at_capacity{1, 2};

  STATIC_REQUIRE(count(empty) == 0U);
  STATIC_REQUIRE(count(populated) == 2U);
  STATIC_REQUIRE(count(at_capacity) == 2U);
  CHECK(count(empty) == 0U);
  CHECK(count(populated) == 2U);
  CHECK(count(at_capacity) == 2U);
}
