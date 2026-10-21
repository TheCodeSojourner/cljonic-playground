#include <catch2/catch_test_macros.hpp>

#include <array>
#include <concepts>
#include <type_traits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {
template <typename T>
concept HasAdmittedNext = requires(const T& value) { requires(!std::same_as<decltype(cljonic::next(value)), void>); };
} // namespace

TEST_CASE("Next returns the logical tail as a bounded Vector", "[next]") {
    using cljonic::count;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::next;
    using cljonic::Queue;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Next");
    TRACE_ID("invariant.Next.NextFreeFunctionSupported");
    TRACE_ID("invariant.Next.SupportsSequenceableCollectionDomain");
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Vector<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Map<int, int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Set<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Queue<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<String<4>>);

    TRACE_ID("invariant.Next.ReturnsBoundedVector");
    TRACE_ID("invariant.Next.ResultCapacityMatchesSource");
    constexpr auto vector = Vector<int, 4>{10, 20, 30};
    constexpr auto vector_tail = next(vector);
    STATIC_REQUIRE((std::same_as<decltype(vector_tail), const Vector<int, 4>>));

    TRACE_ID("invariant.Next.ReturnsElementsAfterFirst");
    STATIC_REQUIRE(count(vector_tail) == 2U);
    STATIC_REQUIRE(vector_tail(0) == 20);
    STATIC_REQUIRE(vector_tail(1) == 30);
    constexpr auto full_vector = Vector<int, 3>{10, 20, 30};
    constexpr auto full_vector_tail = next(full_vector);
    STATIC_REQUIRE((std::same_as<decltype(full_vector_tail), const Vector<int, 3>>));
    STATIC_REQUIRE(count(full_vector_tail) == 2U);
    STATIC_REQUIRE(full_vector_tail(0) == 20);
    STATIC_REQUIRE(full_vector_tail(1) == 30);

    constexpr auto queue = Queue<int, 4>{10, 20, 30};
    constexpr auto queue_tail = next(queue);
    STATIC_REQUIRE((std::same_as<decltype(queue_tail), const Vector<int, 4>>));
    STATIC_REQUIRE(count(queue_tail) == 2U);
    STATIC_REQUIRE(queue_tail(0) == 20);
    STATIC_REQUIRE(queue_tail(1) == 30);
    constexpr auto full_queue = Queue<int, 3>{10, 20, 30};
    constexpr auto full_queue_tail = next(full_queue);
    STATIC_REQUIRE((std::same_as<decltype(full_queue_tail), const Vector<int, 3>>));
    STATIC_REQUIRE(count(full_queue_tail) == 2U);
    STATIC_REQUIRE(full_queue_tail(0) == 20);
    STATIC_REQUIRE(full_queue_tail(1) == 30);

    constexpr auto text = String<4>{"ABC"};
    constexpr auto text_tail = next(text);
    STATIC_REQUIRE((std::same_as<decltype(text_tail), const Vector<char, 4>>));
    STATIC_REQUIRE(count(text_tail) == 2U);
    STATIC_REQUIRE(text_tail(0) == 'B');
    STATIC_REQUIRE(text_tail(1) == 'C');
    constexpr auto full_text = String<3>{"ABC"};
    constexpr auto full_text_tail = next(full_text);
    STATIC_REQUIRE((std::same_as<decltype(full_text_tail), const Vector<char, 3>>));
    STATIC_REQUIRE(count(full_text_tail) == 2U);
    STATIC_REQUIRE(full_text_tail(0) == 'B');
    STATIC_REQUIRE(full_text_tail(1) == 'C');

    TRACE_ID("invariant.Next.MapEntriesRemainValueSemantic");
    constexpr auto map = Map<int, int, 4>{MapEntry{1, 100}, MapEntry{2, 200}, MapEntry{3, 300}};
    constexpr auto map_tail = next(map);
    STATIC_REQUIRE((std::same_as<decltype(map_tail), const Vector<MapEntry<int, int>, 4>>));
    STATIC_REQUIRE(count(map_tail) == 2U);
    STATIC_REQUIRE(map_tail(0) == map.begin()[1]);
    STATIC_REQUIRE(map_tail(1) == map.begin()[2]);
    constexpr auto full_map = Map<int, int, 3>{MapEntry{1, 100}, MapEntry{2, 200}, MapEntry{3, 300}};
    constexpr auto full_map_tail = next(full_map);
    STATIC_REQUIRE((std::same_as<decltype(full_map_tail), const Vector<MapEntry<int, int>, 3>>));
    STATIC_REQUIRE(count(full_map_tail) == 2U);
    STATIC_REQUIRE(full_map_tail(0) == full_map.begin()[1]);
    STATIC_REQUIRE(full_map_tail(1) == full_map.begin()[2]);

    TRACE_ID("invariant.Next.MapSetOrderIsNotSemanticallySignificant");
    constexpr auto set = Set<int, 4>{10, 20, 30};
    constexpr auto set_tail = next(set);
    STATIC_REQUIRE((std::same_as<decltype(set_tail), const Vector<int, 4>>));
    STATIC_REQUIRE(count(set_tail) == 2U);
    STATIC_REQUIRE(set_tail(0) == set.begin()[1]);
    STATIC_REQUIRE(set_tail(1) == set.begin()[2]);
    constexpr auto full_set = Set<int, 3>{10, 20, 30};
    constexpr auto full_set_tail = next(full_set);
    STATIC_REQUIRE((std::same_as<decltype(full_set_tail), const Vector<int, 3>>));
    STATIC_REQUIRE(count(full_set_tail) == 2U);
    STATIC_REQUIRE(full_set_tail(0) == full_set.begin()[1]);
    STATIC_REQUIRE(full_set_tail(1) == full_set.begin()[2]);

    TRACE_ID("invariant.Next.QueuePreservesFifoOrder");
    STATIC_REQUIRE(queue_tail(0) == 20);
    STATIC_REQUIRE(queue_tail(1) == 30);

    TRACE_ID("invariant.Next.EmptyReturnsEmptyVector");
    STATIC_REQUIRE(count(next(Vector<int, 4>{})) == 0U);
    STATIC_REQUIRE(count(next(Map<int, int, 4>{})) == 0U);
    STATIC_REQUIRE(count(next(Set<int, 4>{})) == 0U);
    STATIC_REQUIRE(count(next(Queue<int, 4>{})) == 0U);
    STATIC_REQUIRE(count(next(String<4>{})) == 0U);
    STATIC_REQUIRE(count(next(Vector<int, 4>{10})) == 0U);
    STATIC_REQUIRE(count(next(Map<int, int, 4>{MapEntry{1, 100}})) == 0U);
    STATIC_REQUIRE(count(next(Set<int, 4>{10})) == 0U);
    STATIC_REQUIRE(count(next(Queue<int, 4>{10})) == 0U);
    STATIC_REQUIRE(count(next(String<4>{"A"})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(next(Vector<int, 0>{})), Vector<int, 0>>));
    STATIC_REQUIRE(count(next(Vector<int, 0>{})) == 0U);

    TRACE_ID("invariant.Next.PreservesInput");
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

    TRACE_ID("invariant.Next.NoHeapAllocation");
    TRACE_ID("invariant.Next.NoRtti");
    TRACE_ID("invariant.Next.NoExceptions");
    TRACE_ID("invariant.Next.ConstexprCapable");
    STATIC_REQUIRE(noexcept(next(vector)));
    STATIC_REQUIRE(noexcept(next(map)));
    STATIC_REQUIRE(noexcept(next(set)));
    STATIC_REQUIRE(noexcept(next(queue)));
    STATIC_REQUIRE(noexcept(next(text)));

    TRACE_ID("invariant.Next.RejectsUnsupportedDomainWithTargetedDiagnostic");
    TRACE_ID("invariant.Next.DiagnosticNamesSupportedDomains");
    STATIC_REQUIRE_FALSE(HasAdmittedNext<int>);
    STATIC_REQUIRE_FALSE(HasAdmittedNext<cljonic::MapEntry<int, int>>);
    STATIC_REQUIRE_FALSE(HasAdmittedNext<cljonic::Range<int>>);
    STATIC_REQUIRE(HasAdmittedNext<Vector<int, 4>>);
    STATIC_REQUIRE(HasAdmittedNext<Map<int, int, 4>>);
    STATIC_REQUIRE(HasAdmittedNext<Set<int, 4>>);
    STATIC_REQUIRE(HasAdmittedNext<Queue<int, 4>>);
    STATIC_REQUIRE(HasAdmittedNext<String<4>>);

    volatile int first_raw = 40;
    volatile int second_raw = 50;
    volatile int third_raw = 60;
    const int first = first_raw;
    const int second = second_raw;
    const int third = third_raw;
    const auto runtime_vector = Vector<int, 4>{first, second, third};
    const auto runtime_vector_tail = next(runtime_vector);
    REQUIRE(count(runtime_vector_tail) == 2U);
    REQUIRE(runtime_vector_tail(0) == second);
    REQUIRE(runtime_vector_tail(1) == third);
    REQUIRE(count(runtime_vector) == 3U);
    REQUIRE(runtime_vector(0) == first);

    const auto runtime_queue = Queue<int, 4>{first, second, third};
    const auto runtime_queue_tail = next(runtime_queue);
    REQUIRE(count(runtime_queue_tail) == 2U);
    REQUIRE(runtime_queue_tail(0) == second);
    REQUIRE(runtime_queue_tail(1) == third);
    REQUIRE(count(runtime_queue) == 3U);

    const auto runtime_map = Map<int, int, 4>{MapEntry{first, second}, MapEntry{second, third}};
    const auto runtime_map_tail = next(runtime_map);
    REQUIRE(count(runtime_map_tail) == 1U);
    REQUIRE(runtime_map_tail(0) == runtime_map.begin()[1]);
    REQUIRE(count(runtime_map) == 2U);

    const auto runtime_set = Set<int, 4>{first, second, third};
    const auto runtime_set_tail = next(runtime_set);
    REQUIRE(count(runtime_set_tail) == 2U);
    REQUIRE(runtime_set_tail(0) == runtime_set.begin()[1]);
    REQUIRE(runtime_set_tail(1) == runtime_set.begin()[2]);
    REQUIRE(count(runtime_set) == 3U);

    volatile char first_char_raw = 'a';
    volatile char second_char_raw = 'b';
    volatile char third_char_raw = 'c';
    const char first_char = first_char_raw;
    const char second_char = second_char_raw;
    const char third_char = third_char_raw;
    const std::array runtime_chars{first_char, second_char, third_char};
    const auto runtime_text = String<4>{runtime_chars};
    const auto runtime_text_tail = next(runtime_text);
    REQUIRE(count(runtime_text_tail) == 2U);
    REQUIRE(runtime_text_tail(0) == second_char);
    REQUIRE(runtime_text_tail(1) == third_char);
    REQUIRE(count(runtime_text) == 3U);
}
