#include <catch2/catch_test_macros.hpp>

#if defined(CLJONIC_HAVE_VECTOR_IMPLEMENTATION)
#include <vector.hpp>
#endif

namespace cljonic::spec_model
{

    enum class vector_state
    {
        unclassified,
        empty,
        populated,
        at_capacity,
        invalid,
    };

    struct vector_observation
    {
        int capacity_limit;
        int logical_size;
    };

    constexpr auto classify_vector(vector_observation observation) -> vector_state
    {
        if (observation.capacity_limit < 0)
        {
            return vector_state::invalid;
        }

        if (observation.logical_size < 0)
        {
            return vector_state::invalid;
        }

        if (observation.logical_size > observation.capacity_limit)
        {
            return vector_state::invalid;
        }

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

TEST_CASE("Catch2 infrastructure is active", "[smoke]")
{
    SUCCEED();
}

TEST_CASE("Vector spec examples classify bounded states", "[vector][spec]")
{
    using cljonic::spec_model::classify_vector;
    using cljonic::spec_model::vector_observation;
    using cljonic::spec_model::vector_state;

    SECTION("negative size is invalid")
    {
        CHECK(classify_vector(vector_observation{4, -1}) == vector_state::invalid);
    }

    SECTION("negative capacity is invalid")
    {
        CHECK(classify_vector(vector_observation{-1, 0}) == vector_state::invalid);
    }

    SECTION("overflow size is invalid")
    {
        CHECK(classify_vector(vector_observation{4, 5}) == vector_state::invalid);
    }

    SECTION("zero size is empty")
    {
        CHECK(classify_vector(vector_observation{4, 0}) == vector_state::empty);
    }

    SECTION("interior size is populated")
    {
        CHECK(classify_vector(vector_observation{4, 2}) == vector_state::populated);
    }

    SECTION("full size is at capacity")
    {
        CHECK(classify_vector(vector_observation{4, 4}) == vector_state::at_capacity);
    }
}

TEST_CASE("Production vector integration reflects implementation availability", "[vector][integration]")
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
    SKIP("Production vector implementation is not present yet. Add src/vector.hpp and rerun ctest to activate integration coverage.");
#endif
}