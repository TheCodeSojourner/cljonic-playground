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
    using cljonic::Range;
    using cljonic::Repeat;
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
    STATIC_REQUIRE(HasAdmittedRest<Range<int>>);
    STATIC_REQUIRE(HasAdmittedRest<Repeat<int>>);
    STATIC_REQUIRE(HasAdmittedRest<cljonic::Cycle<Vector<int, 3>>>);
    STATIC_REQUIRE(HasAdmittedRest<decltype(cljonic::iterate([](int value) noexcept { return value + 1; }, 0))>);
    STATIC_REQUIRE(HasAdmittedRest<decltype(cljonic::repeatedly(3, []() noexcept { return 7; }))>);
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

TEST_CASE("Rest forwards to Next for every supported producer", "[rest]") {
    using cljonic::count;
    using cljonic::cycle;
    using cljonic::into;
    using cljonic::iterate;
    using cljonic::next;
    using cljonic::Range;
    using cljonic::repeat;
    using cljonic::repeatedly;
    using cljonic::rest;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Rest");
    TRACE_ID("invariant.Rest.SupportsSequenceableProducerDomain");
    TRACE_ID("invariant.Rest.ReturnsNextResultForSameProducer");
    TRACE_ID("invariant.Rest.ProducerTailBehaviorMatchesNext");
    TRACE_ID("invariant.Rest.ProducerCallbackTimingAndEffectsMatchNext");
    TRACE_ID("invariant.Rest.PreservesInput");
    TRACE_ID("invariant.Rest.NoHeapAllocation");
    TRACE_ID("invariant.Rest.NoRtti");
    TRACE_ID("invariant.Rest.NoExceptions");
    TRACE_ID("invariant.Rest.ConstexprCapable");

    constexpr auto finite_repeat = repeat(3, 7);
    constexpr auto finite_repeat_tail = rest(finite_repeat);
    STATIC_REQUIRE((
        std::same_as<std::remove_cv_t<decltype(finite_repeat_tail)>, std::remove_cv_t<decltype(next(finite_repeat))>>));
    STATIC_REQUIRE(count(finite_repeat_tail) == 2U);
    constexpr auto finite_repeat_values = into(Vector<int, 2>{}, finite_repeat_tail);
    STATIC_REQUIRE(finite_repeat_values(0) == 7);
    STATIC_REQUIRE(finite_repeat_values(1) == 7);
    STATIC_REQUIRE(count(finite_repeat) == 3U);

    constexpr auto unbounded_repeat_tail = rest(repeat(7));
    STATIC_REQUIRE(count(unbounded_repeat_tail) == count(repeat(7)));

    constexpr auto range_source = Range{0, 3};
    constexpr auto range_tail = rest(range_source);
    STATIC_REQUIRE(
        (std::same_as<std::remove_cv_t<decltype(range_tail)>, std::remove_cv_t<decltype(next(range_source))>>));
    STATIC_REQUIRE((into(Vector<int, 2>{}, range_tail) == Vector<int, 2>{1, 2}));

    constexpr auto cycle_source = cycle(Vector<int, 3>{10, 20, 30});
    constexpr auto cycle_tail = rest(cycle_source);
    STATIC_REQUIRE(
        (std::same_as<std::remove_cv_t<decltype(cycle_tail)>, std::remove_cv_t<decltype(next(cycle_source))>>));
    STATIC_REQUIRE((into(Vector<int, 5>{}, cycle_tail) == Vector<int, 5>{20, 30, 10, 20, 30}));
    STATIC_REQUIRE(count(cycle_source) == count(cycle_tail));

    constexpr auto increment = [](int value) noexcept { return value + 1; };
    constexpr auto iterate_source = iterate(increment, 0);
    constexpr auto iterate_tail = rest(iterate_source);
    STATIC_REQUIRE(
        (std::same_as<std::remove_cv_t<decltype(iterate_tail)>, std::remove_cv_t<decltype(next(iterate_source))>>));
    STATIC_REQUIRE((into(Vector<int, 1>{}, iterate_tail) == Vector<int, 1>{1}));

    constexpr auto repeatedly_tail = rest(repeatedly(3, []() noexcept { return 7; }));
    STATIC_REQUIRE(count(repeatedly_tail) == 2U);
    STATIC_REQUIRE((into(Vector<int, 2>{}, repeatedly_tail) == Vector<int, 2>{7, 7}));

    auto iterate_calls = 0;
    const auto runtime_iterate = iterate(
        [&iterate_calls](int value) noexcept {
            ++iterate_calls;
            return value + 1;
        },
        5);
    const auto runtime_iterate_tail = rest(runtime_iterate);
    REQUIRE(iterate_calls == 1);
    REQUIRE((into(Vector<int, 1>{}, runtime_iterate_tail) == Vector<int, 1>{6}));
    REQUIRE(iterate_calls == 2);

    auto repeatedly_calls = 0;
    const auto runtime_repeatedly = repeatedly(3, [&repeatedly_calls]() noexcept { return ++repeatedly_calls; });
    const auto runtime_repeatedly_tail = rest(runtime_repeatedly);
    REQUIRE(repeatedly_calls == 1);
    REQUIRE(count(runtime_repeatedly_tail) == 2U);
    REQUIRE((into(Vector<int, 2>{}, runtime_repeatedly_tail) == Vector<int, 2>{2, 3}));
    REQUIRE(repeatedly_calls == 3);
    REQUIRE(count(runtime_repeatedly) == 3U);
    REQUIRE(noexcept(rest(runtime_iterate)));
    REQUIRE(noexcept(rest(runtime_repeatedly)));
}
