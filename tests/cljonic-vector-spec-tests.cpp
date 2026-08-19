#include <catch2/catch_test_macros.hpp>

#include <cljonic-core.hpp>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Vector construction establishes logical size", "[vector]") {
  using cljonic::Vector;

  TRACE_ID("entity-fields.VectorCollection");
  TRACE_ID("invariant.VectorCollection.CapacityIsNonNegative");
  TRACE_ID("invariant.VectorCollection.LogicalSizeIsNonNegative");
  TRACE_ID("invariant.VectorCollection.LogicalSizeDoesNotExceedCapacity");
  TRACE_ID("invariant.VectorCollection.SupportsLiteralDeducedConstruction");
  TRACE_ID(
      "invariant.VectorCollection.SupportsEmptyExplicitCapacityConstruction");
  TRACE_ID(
      "invariant.VectorCollection.OversizedInitializerIsCompileTimeFailure");
  TRACE_ID("invariant.VectorCollection."
           "OversizedInitializerDiagnosticIdentifiesCapacity");

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
  TRACE_ID("entity-fields.Collection");
  TRACE_ID("invariant.CountRequest.CountRequiresCollectionSubject");
  TRACE_ID("invariant.Collection.LogicalSizeIsNonNegative");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresLogicalSizeMatch");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresEmptyReturnsZero");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresNonNegativeResult");
  TRACE_ID("rule-failure.CountCollectionCardinality.1");
  TRACE_ID("rule-failure.CountCollectionCardinality.2");
  TRACE_ID("rule-failure.CountCollectionCardinality.3");
  TRACE_ID("rule-success.CountCollectionCardinality");
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
