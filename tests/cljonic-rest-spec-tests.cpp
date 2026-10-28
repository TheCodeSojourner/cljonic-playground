#include <catch2/catch_test_macros.hpp>

#include <array>
#include <concepts>
#include <type_traits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {
template <typename T>
concept HasAdmittedRest = requires(const T& value) { requires(!std::same_as<decltype(cljonic::rest(value)), void>); };
} // namespace

TEST_CASE("Rest returns the logical tail as the same collection kind with reduced capacity", "[rest]") {
    using cljonic::count;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::rest;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Rest");
    TRACE_ID("invariant.Rest.RestFreeFunctionSupported");
    TRACE_ID("invariant.Rest.SupportsSequenceableCollectionDomain");
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Vector<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Map<int, int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Set<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Queue<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<String<4>>);

    TRACE_ID("invariant.Rest.ReturnsNextResultForSameCollection");
    TRACE_ID("invariant.Rest.ResultCapacityIsSourceCapacityMinusOneSaturatingAtZero");
    constexpr auto vector = Vector<int, 4>{10, 20, 30};
    constexpr auto vector_rest = rest(vector);
    STATIC_REQUIRE((std::same_as<decltype(vector_rest), const Vector<int, 3>>));
    STATIC_REQUIRE(count(vector_rest) == 2U);

    TRACE_ID("invariant.Rest.ReturnsEveryLogicalElementAfterFirst");
    STATIC_REQUIRE(vector_rest(0) == 20);
    STATIC_REQUIRE(vector_rest(1) == 30);
    constexpr auto full_vector = Vector<int, 3>{10, 20, 30};
    constexpr auto full_vector_rest = rest(full_vector);
    STATIC_REQUIRE((std::same_as<decltype(full_vector_rest), const Vector<int, 2>>));
    STATIC_REQUIRE(count(full_vector_rest) == 2U);
    STATIC_REQUIRE(full_vector_rest(0) == 20);
    STATIC_REQUIRE(full_vector_rest(1) == 30);

    TRACE_ID("invariant.Rest.QueuePreservesFifoOrder");
    constexpr auto queue = Queue<int, 4>{10, 20, 30};
    constexpr auto queue_rest = rest(queue);
    STATIC_REQUIRE((std::same_as<decltype(queue_rest), const Queue<int, 3>>));
    STATIC_REQUIRE(count(queue_rest) == 2U);
    STATIC_REQUIRE(queue_rest.begin()[0] == 20);
    STATIC_REQUIRE(queue_rest.begin()[1] == 30);
    constexpr auto full_queue = Queue<int, 3>{10, 20, 30};
    constexpr auto full_queue_rest = rest(full_queue);
    STATIC_REQUIRE((std::same_as<decltype(full_queue_rest), const Queue<int, 2>>));
    STATIC_REQUIRE(count(full_queue_rest) == 2U);
    STATIC_REQUIRE(full_queue_rest.begin()[0] == 20);
    STATIC_REQUIRE(full_queue_rest.begin()[1] == 30);

    constexpr auto text = String<4>{"ABC"};
    constexpr auto text_rest = rest(text);
    STATIC_REQUIRE((std::same_as<decltype(text_rest), const String<3>>));
    STATIC_REQUIRE(count(text_rest) == 2U);
    STATIC_REQUIRE(text_rest(0) == 'B');
    STATIC_REQUIRE(text_rest(1) == 'C');
    constexpr auto full_text = String<3>{"ABC"};
    constexpr auto full_text_rest = rest(full_text);
    STATIC_REQUIRE((std::same_as<decltype(full_text_rest), const String<2>>));
    STATIC_REQUIRE(count(full_text_rest) == 2U);
    STATIC_REQUIRE(full_text_rest(0) == 'B');
    STATIC_REQUIRE(full_text_rest(1) == 'C');

    TRACE_ID("invariant.Rest.MapEntriesRemainValueSemantic");
    constexpr auto map = Map<int, int, 4>{MapEntry{1, 100}, MapEntry{2, 200}, MapEntry{3, 300}};
    constexpr auto map_rest = rest(map);
    STATIC_REQUIRE((std::same_as<decltype(map_rest), const Map<int, int, 3>>));
    STATIC_REQUIRE(count(map_rest) == 2U);
    STATIC_REQUIRE(map_rest.begin()[0] == map.begin()[1]);
    STATIC_REQUIRE(map_rest.begin()[1] == map.begin()[2]);
    constexpr auto full_map = Map<int, int, 3>{MapEntry{1, 100}, MapEntry{2, 200}, MapEntry{3, 300}};
    constexpr auto full_map_rest = rest(full_map);
    STATIC_REQUIRE((std::same_as<decltype(full_map_rest), const Map<int, int, 2>>));
    STATIC_REQUIRE(count(full_map_rest) == 2U);
    STATIC_REQUIRE(full_map_rest.begin()[0] == full_map.begin()[1]);
    STATIC_REQUIRE(full_map_rest.begin()[1] == full_map.begin()[2]);

    TRACE_ID("invariant.Rest.MapSetOrderIsNotSemanticallySignificant");
    constexpr auto set = Set<int, 4>{10, 20, 30};
    constexpr auto set_rest = rest(set);
    STATIC_REQUIRE((std::same_as<decltype(set_rest), const Set<int, 3>>));
    STATIC_REQUIRE(count(set_rest) == 2U);
    STATIC_REQUIRE(set_rest.begin()[0] == set.begin()[1]);
    STATIC_REQUIRE(set_rest.begin()[1] == set.begin()[2]);
    constexpr auto full_set = Set<int, 3>{10, 20, 30};
    constexpr auto full_set_rest = rest(full_set);
    STATIC_REQUIRE((std::same_as<decltype(full_set_rest), const Set<int, 2>>));
    STATIC_REQUIRE(count(full_set_rest) == 2U);
    STATIC_REQUIRE(full_set_rest.begin()[0] == full_set.begin()[1]);
    STATIC_REQUIRE(full_set_rest.begin()[1] == full_set.begin()[2]);

    TRACE_ID("invariant.Rest.DoesNotDeriveSemanticsFromPop");
    STATIC_REQUIRE(vector_rest(0) == 20);
    STATIC_REQUIRE(vector_rest(1) == 30);

    TRACE_ID("invariant.Rest.EmptyOrSingletonReturnsEmptyResult");
    STATIC_REQUIRE(count(rest(Vector<int, 4>{})) == 0U);
    STATIC_REQUIRE(count(rest(Map<int, int, 4>{})) == 0U);
    STATIC_REQUIRE(count(rest(Set<int, 4>{})) == 0U);
    STATIC_REQUIRE(count(rest(Queue<int, 4>{})) == 0U);
    STATIC_REQUIRE(count(rest(String<4>{})) == 0U);
    STATIC_REQUIRE(count(rest(Vector<int, 4>{10})) == 0U);
    STATIC_REQUIRE(count(rest(Map<int, int, 4>{MapEntry{1, 100}})) == 0U);
    STATIC_REQUIRE(count(rest(Set<int, 4>{10})) == 0U);
    STATIC_REQUIRE(count(rest(Queue<int, 4>{10})) == 0U);
    STATIC_REQUIRE(count(rest(String<4>{"A"})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(rest(Vector<int, 0>{})), Vector<int, 0>>));
    STATIC_REQUIRE(count(rest(Vector<int, 0>{})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(rest(Queue<int, 0>{})), Queue<int, 0>>));
    STATIC_REQUIRE(count(rest(Queue<int, 0>{})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(rest(Set<int, 0>{})), Set<int, 0>>));
    STATIC_REQUIRE(count(rest(Set<int, 0>{})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(rest(Map<int, int, 0>{})), Map<int, int, 0>>));
    STATIC_REQUIRE(count(rest(Map<int, int, 0>{})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(rest(String<0>{})), String<0>>));
    STATIC_REQUIRE(count(rest(String<0>{})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(rest(Queue<int, 1>{10})), Queue<int, 0>>));
    STATIC_REQUIRE(count(rest(Queue<int, 1>{10})) == 0U);

    TRACE_ID("invariant.Rest.PreservesInput");
    STATIC_REQUIRE(count(vector) == 3U);
    STATIC_REQUIRE(vector(0) == 10);
    STATIC_REQUIRE(count(map) == 3U);
    STATIC_REQUIRE(count(set) == 3U);
    STATIC_REQUIRE(count(queue) == 3U);
    STATIC_REQUIRE(count(text) == 3U);
    STATIC_REQUIRE((map.begin()[0] == MapEntry{1, 100}));
    STATIC_REQUIRE(set.begin()[0] == 10);
    STATIC_REQUIRE(queue.begin()[0] == 10);
    STATIC_REQUIRE(text(0) == 'A');

    TRACE_ID("invariant.Rest.NoHeapAllocation");
    TRACE_ID("invariant.Rest.NoRtti");
    TRACE_ID("invariant.Rest.NoExceptions");
    TRACE_ID("invariant.Rest.ConstexprCapable");
    STATIC_REQUIRE(noexcept(rest(vector)));
    STATIC_REQUIRE(noexcept(rest(map)));
    STATIC_REQUIRE(noexcept(rest(set)));
    STATIC_REQUIRE(noexcept(rest(queue)));
    STATIC_REQUIRE(noexcept(rest(text)));

    TRACE_ID("invariant.Rest.RejectsUnsupportedDomainWithTargetedDiagnostic");
    TRACE_ID("invariant.Rest.DiagnosticNamesSupportedDomains");
    STATIC_REQUIRE_FALSE(HasAdmittedRest<int>);
    STATIC_REQUIRE_FALSE(HasAdmittedRest<MapEntry<int, int>>);
    STATIC_REQUIRE_FALSE(HasAdmittedRest<cljonic::Range<int>>);
    STATIC_REQUIRE(HasAdmittedRest<Vector<int, 4>>);
    STATIC_REQUIRE(HasAdmittedRest<Map<int, int, 4>>);
    STATIC_REQUIRE(HasAdmittedRest<Set<int, 4>>);
    STATIC_REQUIRE(HasAdmittedRest<Queue<int, 4>>);
    STATIC_REQUIRE(HasAdmittedRest<String<4>>);

    volatile int first_raw = 40;
    volatile int second_raw = 50;
    volatile int third_raw = 60;
    const int first = first_raw;
    const int second = second_raw;
    const int third = third_raw;

    const auto runtime_vector = Vector<int, 4>{first, second, third};
    const auto runtime_vector_rest = rest(runtime_vector);
    REQUIRE(count(runtime_vector_rest) == 2U);
    REQUIRE(runtime_vector_rest(0) == second);
    REQUIRE(runtime_vector_rest(1) == third);
    REQUIRE(count(runtime_vector) == 3U);
    REQUIRE(runtime_vector(0) == first);

    const auto runtime_queue = Queue<int, 4>{first, second, third};
    const auto runtime_queue_rest = rest(runtime_queue);
    REQUIRE(count(runtime_queue_rest) == 2U);
    REQUIRE(runtime_queue_rest.begin()[0] == second);
    REQUIRE(runtime_queue_rest.begin()[1] == third);
    REQUIRE(count(runtime_queue) == 3U);

    const auto runtime_map = Map<int, int, 4>{MapEntry{first, second}, MapEntry{second, third}, MapEntry{third, first}};
    const auto runtime_map_rest = rest(runtime_map);
    REQUIRE(count(runtime_map_rest) == 2U);
    REQUIRE(runtime_map_rest.begin()[0] == runtime_map.begin()[1]);
    REQUIRE(runtime_map_rest.begin()[1] == runtime_map.begin()[2]);
    REQUIRE(count(runtime_map) == 3U);

    const auto runtime_set = Set<int, 4>{first, second, third};
    const auto runtime_set_rest = rest(runtime_set);
    REQUIRE(count(runtime_set_rest) == 2U);
    REQUIRE(runtime_set_rest.begin()[0] == runtime_set.begin()[1]);
    REQUIRE(runtime_set_rest.begin()[1] == runtime_set.begin()[2]);
    REQUIRE(count(runtime_set) == 3U);

    volatile char first_char_raw = 'a';
    volatile char second_char_raw = 'b';
    volatile char third_char_raw = 'c';
    const char first_char = first_char_raw;
    const char second_char = second_char_raw;
    const char third_char = third_char_raw;
    const std::array runtime_chars{first_char, second_char, third_char};
    const auto runtime_text = String<4>{runtime_chars};
    const auto runtime_text_rest = rest(runtime_text);
    REQUIRE(count(runtime_text_rest) == 2U);
    REQUIRE(runtime_text_rest(0) == second_char);
    REQUIRE(runtime_text_rest(1) == third_char);
    REQUIRE(count(runtime_text) == 3U);
    REQUIRE(runtime_text(0) == first_char);
}
