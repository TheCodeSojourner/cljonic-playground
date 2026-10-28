#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <type_traits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {
template <typename T>
concept HasAdmittedHasNext =
    requires(const T& value) { requires(!std::same_as<decltype(cljonic::has_next(value)), void>); };
} // namespace

TEST_CASE("has_next preflights whether the logical tail is nonempty", "[has_next]") {
    using cljonic::count;
    using cljonic::has_next;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.HasNext");
    TRACE_ID("invariant.HasNext.HasNextFreeFunctionSupported");
    TRACE_ID("invariant.HasNext.SupportsNextCollectionDomain");
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Vector<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Map<int, int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Set<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Queue<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<String<4>>);

    TRACE_ID("invariant.HasNext.ReturnsBooleanPreflight");
    STATIC_REQUIRE(std::same_as<decltype(has_next(Vector<int, 4>{})), bool>);

    TRACE_ID("invariant.HasNext.ReturnsTrueWhenLogicalTailNonempty");
    constexpr auto vector = Vector<int, 4>{10, 20, 30};
    constexpr auto map = Map<int, int, 4>{MapEntry{1, 100}, MapEntry{2, 200}};
    constexpr auto set = Set<int, 4>{10, 20};
    constexpr auto queue = Queue<int, 4>{10, 20};
    constexpr auto text = String<4>{"AB"};
    STATIC_REQUIRE(has_next(vector));
    STATIC_REQUIRE(has_next(map));
    STATIC_REQUIRE(has_next(set));
    STATIC_REQUIRE(has_next(queue));
    STATIC_REQUIRE(has_next(text));

    TRACE_ID("invariant.HasNext.ReturnsFalseForEmptyOrSingletonCollection");
    STATIC_REQUIRE_FALSE(has_next(Vector<int, 4>{}));
    STATIC_REQUIRE_FALSE(has_next(Map<int, int, 4>{}));
    STATIC_REQUIRE_FALSE(has_next(Set<int, 4>{}));
    STATIC_REQUIRE_FALSE(has_next(Queue<int, 4>{}));
    STATIC_REQUIRE_FALSE(has_next(String<4>{}));
    STATIC_REQUIRE_FALSE(has_next(Vector<int, 4>{10}));
    STATIC_REQUIRE_FALSE(has_next(Map<int, int, 4>{MapEntry{1, 100}}));
    STATIC_REQUIRE_FALSE(has_next(Set<int, 4>{10}));
    STATIC_REQUIRE_FALSE(has_next(Queue<int, 4>{10}));
    STATIC_REQUIRE_FALSE(has_next(String<4>{"A"}));
    STATIC_REQUIRE_FALSE(has_next(Vector<int, 0>{}));
    STATIC_REQUIRE_FALSE(has_next(Map<int, int, 0>{}));
    STATIC_REQUIRE_FALSE(has_next(Set<int, 0>{}));
    STATIC_REQUIRE_FALSE(has_next(Queue<int, 0>{}));
    STATIC_REQUIRE_FALSE(has_next(String<0>{}));

    TRACE_ID("invariant.HasNext.ObservesCountWithoutConstructingNextResult");
    STATIC_REQUIRE(has_next(vector) == (count(vector) > 1));
    STATIC_REQUIRE(has_next(map) == (count(map) > 1));
    STATIC_REQUIRE(has_next(set) == (count(set) > 1));
    STATIC_REQUIRE(has_next(queue) == (count(queue) > 1));
    STATIC_REQUIRE(has_next(text) == (count(text) > 1));

    TRACE_ID("invariant.HasNext.PreservesInput");
    STATIC_REQUIRE(count(vector) == 3U);
    STATIC_REQUIRE(vector(0) == 10);
    STATIC_REQUIRE(count(map) == 2U);
    STATIC_REQUIRE(count(set) == 2U);
    STATIC_REQUIRE(count(queue) == 2U);
    STATIC_REQUIRE(count(text) == 2U);

    TRACE_ID("invariant.HasNext.NoHeapAllocation");
    TRACE_ID("invariant.HasNext.NoRtti");
    TRACE_ID("invariant.HasNext.NoExceptions");
    TRACE_ID("invariant.HasNext.ConstexprCapable");
    STATIC_REQUIRE(noexcept(has_next(vector)));
    STATIC_REQUIRE(noexcept(has_next(map)));
    STATIC_REQUIRE(noexcept(has_next(set)));
    STATIC_REQUIRE(noexcept(has_next(queue)));
    STATIC_REQUIRE(noexcept(has_next(text)));

    TRACE_ID("invariant.HasNext.RejectsUnsupportedDomainWithTargetedDiagnostic");
    TRACE_ID("invariant.HasNext.DiagnosticNamesSupportedDomains");
    STATIC_REQUIRE_FALSE(HasAdmittedHasNext<int>);
    STATIC_REQUIRE_FALSE(HasAdmittedHasNext<MapEntry<int, int>>);
    STATIC_REQUIRE_FALSE(HasAdmittedHasNext<cljonic::Range<int>>);
    STATIC_REQUIRE(HasAdmittedHasNext<Vector<int, 4>>);
    STATIC_REQUIRE(HasAdmittedHasNext<Map<int, int, 4>>);
    STATIC_REQUIRE(HasAdmittedHasNext<Set<int, 4>>);
    STATIC_REQUIRE(HasAdmittedHasNext<Queue<int, 4>>);
    STATIC_REQUIRE(HasAdmittedHasNext<String<4>>);

    volatile int raw = 40;
    const int first = raw;
    const int second = first + 1;
    const auto runtime_vector = Vector<int, 4>{first, second};
    const auto runtime_map = Map<int, int, 4>{MapEntry{first, second}, MapEntry{second, first}};
    const auto runtime_set = Set<int, 4>{first, second};
    const auto runtime_queue = Queue<int, 4>{first, second};
    const char runtime_characters[]{static_cast<char>('A' + first - 40), 'B', '\0'};
    const auto runtime_text = String<4>{runtime_characters};

    REQUIRE(has_next(runtime_vector));
    REQUIRE(has_next(runtime_map));
    REQUIRE(has_next(runtime_set));
    REQUIRE(has_next(runtime_queue));
    REQUIRE(has_next(runtime_text));
    REQUIRE_FALSE(has_next(Vector<int, 4>{}));
    REQUIRE_FALSE(has_next(Vector<int, 4>{first}));
    REQUIRE(count(runtime_vector) == 2U);
    REQUIRE(runtime_vector(0) == first);
    REQUIRE(count(runtime_map) == 2U);
    REQUIRE(count(runtime_set) == 2U);
    REQUIRE(count(runtime_queue) == 2U);
    REQUIRE(count(runtime_text) == 2U);
}
