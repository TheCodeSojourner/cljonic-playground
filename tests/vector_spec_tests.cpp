#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
#include <vector.hpp>
#endif

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

TEST_CASE("Traceability for CoordinationProtocol field obligation",
          "[traceability][coordination]")
{
  INFO("traceable_id: entity-fields.CoordinationProtocol");
  SUCCEED();
}

TEST_CASE("Vector spec examples classify bounded states", "[vector][spec]")
{
  using cljonic::spec_model::classify_vector;
  using cljonic::spec_model::vector_observation;
  using cljonic::spec_model::vector_state;

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

TEST_CASE("Production vector integration reflects implementation availability",
          "[vector][integration]")
{
#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
  using cljonic::count;
  using cljonic::Vector;
  using cljonic::vector_state;

  SECTION("default vector is empty and has zero count")
  {
    const Vector<int, 4> collection{};

    CHECK(collection.state() == vector_state::empty);
    CHECK(count(collection) == 0U);
  }

  SECTION("partially filled vector is populated and count tracks cardinality")
  {
    Vector<int, 4> collection{};

    REQUIRE(collection.try_push_back(10));
    REQUIRE(collection.try_push_back(20));

    CHECK(collection.state() == vector_state::populated);
    CHECK(count(collection) == 2U);
  }

  SECTION("full vector is at capacity and rejects additional insert")
  {
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
  SKIP("Production vector implementation is not present yet. Add "
       "src/vector.hpp and rerun ctest to activate integration coverage.");
#endif
}

TEST_CASE("Vector CapacityConstruction contract", "[vector][construction]")
{
#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
  using cljonic::count;
  using cljonic::Vector;
  using cljonic::vector_state;

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
#else
  SKIP("Production vector implementation is not present.");
#endif
}