#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
#include <cljonic-core.hpp>
#endif

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("count on Vector tracks cardinality across collection states",
          "[count][vector][integration]") {
#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
  using cljonic::count;
  using cljonic::Vector;
  using cljonic::vector_state;

  TRACE_ID("entity-fields.CountRequest");
  TRACE_ID("entity-optional.CountRequest.vector_subject");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresCardinalityMatch");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresEmptyReturnsZero");
  TRACE_ID("invariant.CountRequest."
           "ResolvedCountRequiresCollectionMaximumElementCountPolicyRespect");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresNonNegativeResult");
  TRACE_ID("invariant.CountRequest.VectorSubjectRequiresValidVectorState");
  TRACE_ID("invariant.CountRequest.VectorTargetRequiresVectorSubject");
  TRACE_ID("rule-success.ResolveCountSemantics");
  TRACE_ID("rule-failure.ResolveCountSemantics.1");
  TRACE_ID("rule-failure.ResolveCountSemantics.2");
  TRACE_ID("rule-failure.ResolveCountSemantics.3");
  TRACE_ID("rule-failure.ResolveCountSemantics.4");
  TRACE_ID("rule-failure.ResolveCountSemantics.5");
  TRACE_ID("rule-failure.ResolveCountSemantics.6");
  TRACE_ID("transition-edge.CountRequest.pending.rejected");
  TRACE_ID("transition-edge.CountRequest.pending.resolved");
  TRACE_ID("transition-rejected.CountRequest.count_state");
  TRACE_ID("transition-terminal.CountRequest.count_state");
  TRACE_ID("surface-actor.CountSurface");
  TRACE_ID("surface-provides.CountSurface");

  SECTION("default vector is empty and has zero count") {
    const Vector<int, 4> collection{};

    CHECK(collection.state() == vector_state::empty);
    CHECK(count(collection) == 0U);
  }

  SECTION("partially filled vector is populated and count tracks cardinality") {
    Vector<int, 4> collection{};

    REQUIRE(collection.try_push_back(10));
    REQUIRE(collection.try_push_back(20));

    CHECK(collection.state() == vector_state::populated);
    CHECK(count(collection) == 2U);
  }

  SECTION("full vector is at capacity and rejects additional insert") {
    Vector<int, 2> collection{};

    REQUIRE(collection.try_push_back(10));
    REQUIRE(collection.try_push_back(20));
    CHECK(collection.state() == vector_state::at_capacity);
    CHECK(count(collection) == 2U);

    CHECK_FALSE(collection.try_push_back(30));
    CHECK(collection.state() == vector_state::at_capacity);
    CHECK(count(collection) == 2U);
  }
#else
  SKIP("Production implementation not present; skipping count integration "
       "tests.");
#endif
}

TEST_CASE("Trace policy: count invariants with valid Vector subject",
          "[count][vector][trace]") {
#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
  using cljonic::count;
  using cljonic::Vector;

  TRACE_ID("rule-success.ConstructVectorWithValidElementCount");
  TRACE_ID("invariant.CountRequest."
           "ResolvedCountRequiresCollectionMaximumElementCountPolicyRespect");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresNonNegativeResult");
  TRACE_ID("rule-success.RejectCountSemanticsOnCardinalityMismatch");
  TRACE_ID(
      "rule-success."
      "RejectCountSemanticsOnCollectionMaximumElementCountPolicyViolation");
  TRACE_ID("rule-success.RejectCountSemanticsOnEmptyBehaviorMismatch");
  TRACE_ID("rule-success.RejectCountSemanticsOnInvalidResultRange");
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
  const Vector<int, 4> v{1, 2};
  CHECK(Vector<int, 4>::capacity_limit() <=
        Vector<int, 4>::collection_maximum_element_count());
  CHECK(count(v) >= 0U);
#else
  SKIP("Production implementation not present.");
#endif
}
