#include <catch2/catch_test_macros.hpp>

#include <concepts>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {
template <typename C>
constexpr auto preserves_owning_value(C input) noexcept -> bool {
    const auto original = input;
    auto result = cljonic::not_empty(input);
    static_assert(std::same_as<decltype(cljonic::not_empty(input)), C>);
    static_assert(noexcept(cljonic::not_empty(input)));
    if (!cljonic::equal(result, original) || !cljonic::equal(input, original)) {
        return false;
    }
    input = C{};
    if (!cljonic::equal(result, original)) {
        return false;
    }
    result = C{};
    return cljonic::is_empty(result) && cljonic::equal(cljonic::not_empty(original), original);
}
} // namespace

TEST_CASE("NotEmpty preserves owning collection values", "[not_empty]") {
    using namespace cljonic;
    TRACE_ID("entity-fields.NotEmpty");
    TRACE_ID("invariant.NotEmpty.SupportsSequenceableCollectionDomain");
    TRACE_ID("invariant.NotEmpty.ReturnsIndependentOwningCopyWhenNonempty");
    TRACE_ID("invariant.NotEmpty.PreservesCollectionTypeAndCapacity");
    TRACE_ID("invariant.NotEmpty.PreservesAllLogicalElements");
    TRACE_ID("invariant.NotEmpty.LeavesInputUnchanged");
    TRACE_ID("invariant.NotEmpty.SupportsConstantEvaluation");
    TRACE_ID("invariant.NotEmpty.IsNoexcept");
    TRACE_ID("invariant.NotEmpty.NoHeapAllocation");
    TRACE_ID("invariant.NotEmpty.NoRtti");
    TRACE_ID("invariant.NotEmpty.NoExceptions");
    TRACE_ID("invariant.NotEmpty.SingleThreadedExecutionModel");
    TRACE_ID("invariant.NotEmpty.ReferentialTransparency");

    STATIC_REQUIRE(preserves_owning_value(Vector<int, 4>{1, 2, 3}));
    STATIC_REQUIRE(preserves_owning_value(Map<int, int, 4>{MapEntry{1, 10}, MapEntry{2, 20}}));
    STATIC_REQUIRE(preserves_owning_value(Set<int, 4>{1, 2, 3}));
    STATIC_REQUIRE(preserves_owning_value(Queue<int, 4>{1, 2, 3}));
    STATIC_REQUIRE(preserves_owning_value(String<4>{"abc"}));
    STATIC_REQUIRE(preserves_owning_value(Vector{Vector{1, 2}, Vector{3, 4}}));
    STATIC_REQUIRE(preserves_owning_value(Queue<int, 3>{1, 2, 3}.pop().conj(4)));
    STATIC_REQUIRE(preserves_owning_value(Vector{0}));
    STATIC_REQUIRE(preserves_owning_value(Map{MapEntry{0, 0}}));
    STATIC_REQUIRE(preserves_owning_value(Set{0}));
    STATIC_REQUIRE(preserves_owning_value(Queue{0}));

    volatile int raw = 7;
    const int value = raw;
    REQUIRE(preserves_owning_value(Vector<int, 4>{value, 2, 3}));
    REQUIRE(preserves_owning_value(Map<int, int, 4>{MapEntry{1, value}, MapEntry{2, 20}}));
    REQUIRE(preserves_owning_value(Set<int, 4>{value, 2, 3}));
    REQUIRE(preserves_owning_value(Queue<int, 4>{value, 2, 3}));
    volatile char raw_character = 'a';
    const char character = raw_character;
    const char characters[]{character, 'b', 'c', '\0'};
    REQUIRE(preserves_owning_value(String<4>{characters}));
    REQUIRE(preserves_owning_value(Vector{Vector{value, 2}, Vector{3, 4}}));
    REQUIRE(preserves_owning_value(Queue<int, 3>{1, 2, 3}.pop().conj(value)));
    const int default_value = value - value;
    REQUIRE(preserves_owning_value(Vector{default_value}));
    REQUIRE(preserves_owning_value(Map{MapEntry{default_value, default_value}}));
    REQUIRE(preserves_owning_value(Set{default_value}));
    REQUIRE(preserves_owning_value(Queue{default_value}));
}

TEST_CASE("NotEmpty handles empty zero-capacity and full collections", "[not_empty]") {
    using namespace cljonic;
    TRACE_ID("invariant.NotEmpty.ReturnsEmptyOwningValueWhenEmpty");
    TRACE_ID("invariant.NotEmpty.SupportsZeroCapacityAndFullCollections");

    STATIC_REQUIRE(preserves_owning_value(Vector<int, 4>{}));
    STATIC_REQUIRE(preserves_owning_value(Map<int, int, 4>{}));
    STATIC_REQUIRE(preserves_owning_value(Set<int, 4>{}));
    STATIC_REQUIRE(preserves_owning_value(Queue<int, 4>{}));
    STATIC_REQUIRE(preserves_owning_value(String<4>{}));
    STATIC_REQUIRE(preserves_owning_value(Vector<int, 0>{}));
    STATIC_REQUIRE(preserves_owning_value(Map<int, int, 0>{}));
    STATIC_REQUIRE(preserves_owning_value(Set<int, 0>{}));
    STATIC_REQUIRE(preserves_owning_value(Queue<int, 0>{}));
    STATIC_REQUIRE(preserves_owning_value(String<0>{}));
    STATIC_REQUIRE(preserves_owning_value(Vector<int, 1>{1}));
    STATIC_REQUIRE(preserves_owning_value(Map<int, int, 1>{MapEntry{1, 2}}));
    STATIC_REQUIRE(preserves_owning_value(Set<int, 1>{1}));
    STATIC_REQUIRE(preserves_owning_value(Queue<int, 1>{1}));
    STATIC_REQUIRE(preserves_owning_value(String<1>{"a"}));

    REQUIRE(preserves_owning_value(Vector<int, 4>{}));
    REQUIRE(preserves_owning_value(Map<int, int, 4>{}));
    REQUIRE(preserves_owning_value(Set<int, 4>{}));
    REQUIRE(preserves_owning_value(Queue<int, 4>{}));
    REQUIRE(preserves_owning_value(String<4>{}));
    REQUIRE(preserves_owning_value(Vector<int, 0>{}));
    REQUIRE(preserves_owning_value(Map<int, int, 0>{}));
    REQUIRE(preserves_owning_value(Set<int, 0>{}));
    REQUIRE(preserves_owning_value(Queue<int, 0>{}));
    REQUIRE(preserves_owning_value(String<0>{}));
    volatile int raw = 9;
    const int value = raw;
    REQUIRE(preserves_owning_value(Vector<int, 1>{value}));
    REQUIRE(preserves_owning_value(Map<int, int, 1>{MapEntry{1, value}}));
    REQUIRE(preserves_owning_value(Set<int, 1>{value}));
    REQUIRE(preserves_owning_value(Queue<int, 1>{value}));
    volatile char raw_character = 'x';
    const char character = raw_character;
    const char characters[]{character, '\0'};
    REQUIRE(preserves_owning_value(String<1>{characters}));
}

TEST_CASE("NotEmpty admission is collections only", "[not_empty]") {
    using namespace cljonic;
    TRACE_ID("invariant.NotEmpty.RejectsUnsupportedDomainWithTargetedDiagnostic");
    STATIC_REQUIRE_FALSE(concepts::SequenceableCollection<int>);
    STATIC_REQUIRE_FALSE(concepts::SequenceableCollection<Range<int>>);
    STATIC_REQUIRE_FALSE(concepts::SequenceableCollection<Repeat<int>>);
    STATIC_REQUIRE_FALSE(concepts::SequenceableCollection<MapEntry<int, int>>);
    STATIC_REQUIRE_FALSE(concepts::SequenceableCollection<int[2]>);
}
