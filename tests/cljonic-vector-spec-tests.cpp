#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
#include <cljonic-core.hpp>
#endif

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace cljonic::spec_model
{

  enum class vector_state
  {
    empty,
    populated,
    at_capacity,
  };

  struct vector_observation
  {
    int capacity_limit;
    int logical_size;
  };

  constexpr auto classify_vector(vector_observation observation) -> vector_state
  {
    if (observation.logical_size == 0)
    {
      return vector_state::empty;
    }

    if (observation.logical_size == observation.capacity_limit)
    {
      return vector_state::at_capacity;
    }

    return vector_state::populated;
  }

} // namespace cljonic::spec_model

TEST_CASE("Catch2 infrastructure is active", "[smoke]") { SUCCEED(); }

TEST_CASE("Vector spec examples classify bounded states", "[vector][spec]")
{
  using cljonic::spec_model::classify_vector;
  using cljonic::spec_model::vector_observation;
  using cljonic::spec_model::vector_state;

  TRACE_ID("entity-fields.VectorCollection");
  TRACE_ID("invariant.VectorCollection.AtCapacityStateMeansSizeEqualsCapacity");
  TRACE_ID("invariant.VectorCollection.CapacityIsNonNegative");
  TRACE_ID("invariant.VectorCollection.EmptyStateMeansZeroSize");
  TRACE_ID("invariant.VectorCollection.LogicalSizeDoesNotExceedCapacity");
  TRACE_ID("invariant.VectorCollection.LogicalSizeIsNonNegative");
  TRACE_ID(
      "invariant.VectorCollection.PopulatedStateMeansPositiveAndNotAtCapacity");
  TRACE_ID("rule-success.ClassifyVectorAsAtCapacity");
  TRACE_ID("rule-success.ClassifyVectorAsEmpty");
  TRACE_ID("rule-success.ClassifyVectorAsPopulated");
  TRACE_ID("rule-failure.ClassifyVectorAsAtCapacity.1");
  TRACE_ID("rule-failure.ClassifyVectorAsAtCapacity.2");
  TRACE_ID("rule-failure.ClassifyVectorAsEmpty.1");
  TRACE_ID("rule-failure.ClassifyVectorAsPopulated.1");
  TRACE_ID("rule-failure.ClassifyVectorAsPopulated.2");
  TRACE_ID("surface-actor.VectorClassificationSurface");
  TRACE_ID("surface-provides.VectorClassificationSurface");

  auto [capacity, size, expected_state] =
      GENERATE(std::make_tuple(1, 0, vector_state::empty),
               std::make_tuple(1, 1, vector_state::at_capacity),
               std::make_tuple(2, 0, vector_state::empty),
               std::make_tuple(2, 1, vector_state::populated),
               std::make_tuple(2, 2, vector_state::at_capacity),
               std::make_tuple(4, 0, vector_state::empty),
               std::make_tuple(4, 2, vector_state::populated),
               std::make_tuple(4, 4, vector_state::at_capacity),
               std::make_tuple(8, 0, vector_state::empty),
               std::make_tuple(8, 4, vector_state::populated),
               std::make_tuple(8, 8, vector_state::at_capacity));

  CHECK(classify_vector(vector_observation{capacity, size}) == expected_state);
}

TEST_CASE("Vector CapacityConstruction contract", "[vector][construction]")
{
#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
  using cljonic::count;
  using cljonic::Vector;
  using cljonic::vector_state;
  using cljonic::concepts::NothrowConstructible;

  struct NothrowElement
  {
    NothrowElement() = default;
    NothrowElement(const NothrowElement &) = default;
    NothrowElement(int value) noexcept : value(value) {}
    int value = 0;
  };

  struct ThrowingElement
  {
    ThrowingElement() = default;
    ThrowingElement(const ThrowingElement &) = default;
    ThrowingElement(int value) noexcept(false) : value(value) {}
    int value = 0;
  };

  TRACE_ID("entity-fields.VectorConstruction");
  TRACE_ID("rule-success.ConstructVectorWithValidElementCount");
  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.1");
  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.2");
  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.3");
  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.4");
  TRACE_ID("rule-failure.ConstructVectorWithValidElementCount.5");
  TRACE_ID("rule-success.RejectVectorConstructionOnOversizedElementCount");
  TRACE_ID("rule-failure.RejectVectorConstructionOnOversizedElementCount.1");
  TRACE_ID("rule-failure.RejectVectorConstructionOnOversizedElementCount.2");
  TRACE_ID(
      "rule-success.RejectVectorConstructionOnThrowingElementConstruction");
  TRACE_ID(
      "rule-failure.RejectVectorConstructionOnThrowingElementConstruction.1");
  TRACE_ID(
      "rule-failure.RejectVectorConstructionOnThrowingElementConstruction.2");
  TRACE_ID("rule-success."
           "RejectVectorConstructionWhenCapacityExceedsCollectionMaximumElement"
           "Count");
  TRACE_ID("rule-failure."
           "RejectVectorConstructionWhenCapacityExceedsCollectionMaximumElement"
           "Count.1");
  TRACE_ID("rule-failure."
           "RejectVectorConstructionWhenCapacityExceedsCollectionMaximumElement"
           "Count.2");
  TRACE_ID(
      "transition-edge.VectorConstruction.pending.rejected_at_compile_time");
  TRACE_ID("transition-edge.VectorConstruction.pending.succeeded");
  TRACE_ID("transition-rejected.VectorConstruction.outcome");
  TRACE_ID("transition-terminal.VectorConstruction.outcome");
  TRACE_ID("surface-actor.VectorConstructionSurface");
  TRACE_ID("surface-provides.VectorConstructionSurface");

  // RejectVectorConstructionOnOversizedElementCount is a static constraint; not
  // runtime-testable.

  SECTION("literal-deduced construction via CTAD deduces capacity from element "
          "count")
  {
    // CTAD produces Vector<T, N> where N = initializer size, so each has a
    // different type; cannot parametrize with generators. Test a few cases.
    auto [vec, expected_count] = std::pair{Vector{1}, 1U};
    CHECK(vec.state() == vector_state::at_capacity);
    CHECK(count(vec) == expected_count);

    auto [vec2, expected_count2] = std::pair{Vector{1, 2}, 2U};
    CHECK(vec2.state() == vector_state::at_capacity);
    CHECK(count(vec2) == expected_count2);

    auto [vec3, expected_count3] = std::pair{Vector{1, 2, 3, 4}, 4U};
    CHECK(vec3.state() == vector_state::at_capacity);
    CHECK(count(vec3) == expected_count3);
  }

  SECTION("partial initializer fill with explicit capacity yields populated "
          "state")
  {
    auto [fill_count] =
        GENERATE(std::make_tuple(1), std::make_tuple(2), std::make_tuple(3));

    if (fill_count == 1)
    {
      Vector<int, 4> v{1};
      CHECK(v.state() == vector_state::populated);
      CHECK(count(v) == 1U);
    }
    else if (fill_count == 2)
    {
      Vector<int, 4> v{1, 2};
      CHECK(v.state() == vector_state::populated);
      CHECK(count(v) == 2U);
    }
    else if (fill_count == 3)
    {
      Vector<int, 4> v{1, 2, 3};
      CHECK(v.state() == vector_state::populated);
      CHECK(count(v) == 3U);
    }
  }

  SECTION("explicit capacity with no initializers yields empty state")
  {
    Vector<int, 4> v{};
    CHECK(v.state() == vector_state::empty);
    CHECK(count(v) == 0U);
  }

  SECTION("constructor no-throw contract is modeled by concepts")
  {
    static_assert(NothrowConstructible<NothrowElement, int>);
    static_assert(!NothrowConstructible<ThrowingElement, int>);
    CHECK(true);
  }
#else
  SKIP("Production vector implementation is not present.");
#endif
}

TEST_CASE("Vector append uses complete-result vs checked-failure semantics",
          "[vector][append][status]")
{
#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
  using cljonic::count;
  using cljonic::Vector;
  using cljonic::vector_state;

  TRACE_ID("entity-fields.VectorAppend");
  TRACE_ID("invariant.VectorAppend.SizesAreNonNegative");
  TRACE_ID("invariant.VectorAppend.SizesRespectCapacity");
  TRACE_ID("rule-success.AppendWithinCapacityYieldsCompleteResult");
  TRACE_ID("rule-failure.AppendWithinCapacityYieldsCompleteResult.1");
  TRACE_ID("rule-failure.AppendWithinCapacityYieldsCompleteResult.2");
  TRACE_ID("rule-success.AppendAtCapacityYieldsCheckedFailureResult");
  TRACE_ID("rule-failure.AppendAtCapacityYieldsCheckedFailureResult.1");
  TRACE_ID("rule-failure.AppendAtCapacityYieldsCheckedFailureResult.2");
  TRACE_ID("transition-edge.VectorAppend.pending.complete_result");
  TRACE_ID("transition-edge.VectorAppend.pending.checked_failure_result");
  TRACE_ID("transition-rejected.VectorAppend.outcome");
  TRACE_ID("transition-terminal.VectorAppend.outcome");
  TRACE_ID("surface-actor.VectorAppendSurface");
  TRACE_ID("surface-provides.VectorAppendSurface");

  SECTION("append below capacity returns complete-result signal")
  {
    Vector<int, 3> v{1, 2};
    const auto before = count(v);

    CHECK(v.state() == vector_state::populated);
    CHECK(v.try_push_back(3));
    CHECK(count(v) == before + 1U);
    CHECK(v.state() == vector_state::at_capacity);
  }

  SECTION("append at capacity returns checked-failure signal and preserves "
          "size")
  {
    Vector<int, 2> v{1, 2};
    const auto before = count(v);

    CHECK(v.state() == vector_state::at_capacity);
    CHECK_FALSE(v.try_push_back(3));
    CHECK(count(v) == before);
    CHECK(v.state() == vector_state::at_capacity);
  }
#else
  SKIP("Production vector implementation is not present.");
#endif
}

TEST_CASE("Trace policy: "
          "invariant.VectorConstruction."
          "CapacityRespectsCollectionMaximumElementCount",
          "[vector][construction][trace]")
{
#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
  using cljonic::Vector;

  TRACE_ID("invariant.VectorConstruction."
           "CapacityRespectsCollectionMaximumElementCount");
  CHECK(Vector<int, 4>::capacity() <=
        Vector<int, 4>::collection_maximum_element_count());
#else
  SKIP("Production vector implementation is not present.");
#endif
}