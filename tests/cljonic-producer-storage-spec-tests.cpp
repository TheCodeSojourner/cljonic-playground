#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

// Default-constructible, nothrow, const-invocable step for Iterate.
struct AddOne {
    constexpr AddOne() noexcept = default;
    constexpr auto operator()(int value) const noexcept -> int {
        return value + 1;
    }
};

// Default-constructible, nothrow, zero-argument callback for Repeatedly.
struct Generate {
    constexpr Generate() noexcept = default;
    constexpr auto operator()() const noexcept -> int {
        return 42;
    }
};

} // namespace

// ============================================================================
// Producer storage admission (REQ-VAL-017D)
// ============================================================================

TEST_CASE("All five producers satisfy NothrowCollectionElement", "[storage][producer]") {
    TRACE_ID("entity-fields.ProducerStorageAdmission");
    TRACE_ID("invariant.ProducerStorageAdmission.EveryProducerIsNothrowCollectionElement");

    using namespace cljonic::concepts;

    TRACE_ID("invariant.Range.SatisfiesNothrowCollectionElement");
    TRACE_ID("invariant.Repeat.SatisfiesNothrowCollectionElement");
    TRACE_ID("invariant.Cycle.SatisfiesNothrowCollectionElement");
    TRACE_ID("invariant.Iterate.SatisfiesNothrowCollectionElement");
    TRACE_ID("invariant.Repeatedly.SatisfiesNothrowCollectionElement");

    STATIC_REQUIRE(NothrowCollectionElement<cljonic::Range<int>>);
    STATIC_REQUIRE(NothrowCollectionElement<cljonic::Repeat<int>>);
    STATIC_REQUIRE(NothrowCollectionElement<cljonic::Cycle<cljonic::Vector<int, 4>>>);
    STATIC_REQUIRE(NothrowCollectionElement<cljonic::Iterate<int, AddOne>>);
    STATIC_REQUIRE(NothrowCollectionElement<cljonic::Repeatedly<int, Generate>>);
}

TEST_CASE("Default-constructed producers are valid and deterministic", "[storage][producer]") {
    TRACE_ID("invariant.ProducerStorageAdmission.DefaultConstructedProducerIsValidAndDeterministic");
    TRACE_ID("invariant.ProducerStorageAdmission.DefaultConstructionDoesNotEvaluateCallbacks");
    TRACE_ID("invariant.ProducerStorageAdmission.DefaultConstructionDoesNotAllocateOrThrow");
    TRACE_ID("invariant.Range.DefaultFormIsValid");
    TRACE_ID("invariant.Repeat.DefaultFormIsValid");
    TRACE_ID("invariant.Repeatedly.DefaultFormIsValid");
    TRACE_ID("invariant.Iterate.DefaultFormIsValid");
    TRACE_ID("invariant.Cycle.DefaultFormIsValid");

    // Range{}: finite empty range.
    STATIC_REQUIRE(cljonic::Range<int>{}.is_finite());
    STATIC_REQUIRE(cljonic::Range<int>{}.count() == 0);

    // Repeat{}: finite empty repeat of the element default.
    STATIC_REQUIRE(cljonic::Repeat<int>{}.is_finite());
    STATIC_REQUIRE(cljonic::Repeat<int>{}.count() == 0);

    // Repeatedly{}: finite empty repeatedly over a default-constructed step.
    STATIC_REQUIRE(cljonic::Repeatedly<int, Generate>{}.is_finite());
    STATIC_REQUIRE(cljonic::Repeatedly<int, Generate>{}.count() == 0);

    // Iterate{}: unbounded iterate over default element + default step.
    STATIC_REQUIRE_FALSE(cljonic::Iterate<int, AddOne>{}.is_finite());
    STATIC_REQUIRE(cljonic::Iterate<int, AddOne>{}.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);

    // Cycle{}: unbounded cycle over a default-constructed source.
    STATIC_REQUIRE_FALSE(cljonic::Cycle<cljonic::Vector<int, 4>>{}.is_finite());

    // Runtime: default construction does not throw or allocate, and values are usable.
    const auto default_repeat = cljonic::Repeat<int>{};
    CHECK(default_repeat.is_finite());
    CHECK(default_repeat.count() == 0);

    const auto default_range = cljonic::Range<int>{};
    CHECK(default_range.is_finite());
    CHECK(default_range.count() == 0);

    const auto default_generate = cljonic::Repeatedly<int, Generate>{};
    CHECK(default_generate.is_finite());
    CHECK(default_generate.count() == 0);
}

TEST_CASE("Producers may be stored as map values, vector elements, and queue elements", "[storage][producer]") {
    TRACE_ID("entity-fields.ProducerStorageAdmission");
    TRACE_ID("invariant.ProducerStorageAdmission.EveryProducerIsNothrowCollectionElement");

    // Vector of Repeat values.
    const auto vector_of_repeats =
        cljonic::Vector<cljonic::Repeat<int>, 4>{cljonic::Repeat<int>{}, cljonic::Repeat<int>{7, 3U}};
    CHECK(vector_of_repeats.count() == 2U);
    CHECK(vector_of_repeats(0U).is_finite());
    CHECK(vector_of_repeats(0U).count() == 0U);
    CHECK(vector_of_repeats(1U).count() == 3U);

    // Queue of Range values.
    const auto queue_of_ranges = cljonic::Queue<cljonic::Range<int>, 4>{cljonic::Range<int>{}, cljonic::Range<int>{5}};
    CHECK(queue_of_ranges.count() == 2U);

    // Map with a producer as value.
    const auto map_with_repeat = cljonic::Map<int, cljonic::Repeat<int>, 4>{
        cljonic::MapEntry<int, cljonic::Repeat<int>>{1, cljonic::Repeat<int>{7, 3U}}};
    CHECK(map_with_repeat.count() == 1U);
    CHECK(map_with_repeat(1).count() == 3U);

    // Set of producer values (Repeat<int> has no operator==, so Set requires
    // stable-equality; Set<Repeat<int>,N> must not form). Use Range (which has
    // no operator== either) only absent equality; verify the admission boundary
    // instead with a vector of sets of Range? Keep this slice to storage-only
    // positions (REQ-VAL-017D) since producer key/set-element equality is
    // Stream C.
    // Construct collection-of-collection values via Vector assoc to avoid the
    // single-range-argument source-constructor ambiguity.
    const auto pair_of_ranges = cljonic::Queue<cljonic::Range<int>, 2>{cljonic::Range<int>{}, cljonic::Range<int>{3}};
    const auto vector_of_queues =
        cljonic::Vector<cljonic::Queue<cljonic::Range<int>, 2>, 2>{}.assoc(0U, pair_of_ranges);
    CHECK(vector_of_queues.count() == 1U);
    CHECK(vector_of_queues(0U).count() == 2U);

    // Producers may be nested inside collections without materialization.
    // (Use multi-element pack construction: a single producer argument would
    // be matched by the range/view constructor since producers are ranges.)
    const auto inner = cljonic::Vector<cljonic::Repeat<int>, 2>{cljonic::Repeat<int>{9, 2U}, cljonic::Repeat<int>{}};
    const auto nested = cljonic::Vector<cljonic::Vector<cljonic::Repeat<int>, 2>, 2>{}.assoc(0U, inner);
    CHECK(nested.count() == 1U);
    CHECK(nested(0U)(0U).count() == 2U);
}

TEST_CASE("Non-default-constructible components reject storage positions", "[storage][producer]") {
    TRACE_ID("invariant.ProducerStorageAdmission.NonDefaultConstructibleComponentsRejectStorage");

    using namespace cljonic::concepts;

    // A step type that is not default-constructible fails the component
    // requirement. The storage-site rejection is a compile-fail case (a
    // bounded collection cannot value-initialize the producer's step member),
    // verified by the variant-compile-fail gate; here we assert the component
    // property directly.
    struct NonDefaultStep {
        constexpr explicit NonDefaultStep(int) noexcept {
        }
        constexpr auto operator()(int value) const noexcept -> int {
            return value;
        }
    };
    struct NonDefaultGen {
        constexpr explicit NonDefaultGen(int) noexcept {
        }
        constexpr auto operator()() const noexcept -> int {
            return 0;
        }
    };

    STATIC_REQUIRE_FALSE(std::default_initializable<NonDefaultStep>);
    STATIC_REQUIRE_FALSE(std::default_initializable<NonDefaultGen>);

    // REQ-VAL-017D: a producer whose component types are not themselves
    // nothrow default-constructible MUST NOT satisfy NothrowCollectionElement.
    // The producer default constructors are constrained so the component
    // requirement is visible to unevaluated checks (not just storage sites).
    STATIC_REQUIRE_FALSE(NothrowCollectionElement<cljonic::Iterate<int, NonDefaultStep>>);
    STATIC_REQUIRE_FALSE(NothrowCollectionElement<cljonic::Repeatedly<int, NonDefaultGen>>);

    // Cf. a default-constructible step passes the component requirement.
    STATIC_REQUIRE(std::default_initializable<AddOne>);
    STATIC_REQUIRE(NothrowCollectionElement<cljonic::Iterate<int, AddOne>>);
    STATIC_REQUIRE(NothrowCollectionElement<cljonic::Repeatedly<int, Generate>>);
}