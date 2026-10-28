#include <catch2/catch_test_macros.hpp>

#include <array>
#include <concepts>
#include <limits>
#include <type_traits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {
template <typename T>
concept HasAdmittedNext = requires(const T& value) { requires(!std::same_as<decltype(cljonic::next(value)), void>); };
} // namespace

TEST_CASE("Next returns the logical tail as the same collection kind with reduced capacity", "[next]") {
    using cljonic::count;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::next;
    using cljonic::Queue;
    using cljonic::Repeat;
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

    TRACE_ID("invariant.Next.ReturnsSameCollectionKindAsSource");
    TRACE_ID("invariant.Next.ResultCapacityIsSourceCapacityMinusOneSaturatingAtZero");
    constexpr auto vector = Vector<int, 4>{10, 20, 30};
    constexpr auto vector_tail = next(vector);
    STATIC_REQUIRE((std::same_as<decltype(vector_tail), const Vector<int, 3>>));

    TRACE_ID("invariant.Next.ReturnsElementsAfterFirst");
    STATIC_REQUIRE(count(vector_tail) == 2U);
    STATIC_REQUIRE(vector_tail(0) == 20);
    STATIC_REQUIRE(vector_tail(1) == 30);
    constexpr auto full_vector = Vector<int, 3>{10, 20, 30};
    constexpr auto full_vector_tail = next(full_vector);
    STATIC_REQUIRE((std::same_as<decltype(full_vector_tail), const Vector<int, 2>>));
    STATIC_REQUIRE(count(full_vector_tail) == 2U);
    STATIC_REQUIRE(full_vector_tail(0) == 20);
    STATIC_REQUIRE(full_vector_tail(1) == 30);

    constexpr auto queue = Queue<int, 4>{10, 20, 30};
    constexpr auto queue_tail = next(queue);
    STATIC_REQUIRE((std::same_as<decltype(queue_tail), const Queue<int, 3>>));
    STATIC_REQUIRE(count(queue_tail) == 2U);
    STATIC_REQUIRE(queue_tail.begin()[0] == 20);
    STATIC_REQUIRE(queue_tail.begin()[1] == 30);
    constexpr auto full_queue = Queue<int, 3>{10, 20, 30};
    constexpr auto full_queue_tail = next(full_queue);
    STATIC_REQUIRE((std::same_as<decltype(full_queue_tail), const Queue<int, 2>>));
    STATIC_REQUIRE(count(full_queue_tail) == 2U);
    STATIC_REQUIRE(full_queue_tail.begin()[0] == 20);
    STATIC_REQUIRE(full_queue_tail.begin()[1] == 30);

    constexpr auto text = String<4>{"ABC"};
    constexpr auto text_tail = next(text);
    STATIC_REQUIRE((std::same_as<decltype(text_tail), const String<3>>));
    STATIC_REQUIRE(count(text_tail) == 2U);
    STATIC_REQUIRE(text_tail(0) == 'B');
    STATIC_REQUIRE(text_tail(1) == 'C');
    constexpr auto full_text = String<3>{"ABC"};
    constexpr auto full_text_tail = next(full_text);
    STATIC_REQUIRE((std::same_as<decltype(full_text_tail), const String<2>>));
    STATIC_REQUIRE(count(full_text_tail) == 2U);
    STATIC_REQUIRE(full_text_tail(0) == 'B');
    STATIC_REQUIRE(full_text_tail(1) == 'C');

    TRACE_ID("invariant.Next.MapEntriesRemainValueSemantic");
    constexpr auto map = Map<int, int, 4>{MapEntry{1, 100}, MapEntry{2, 200}, MapEntry{3, 300}};
    constexpr auto map_tail = next(map);
    STATIC_REQUIRE((std::same_as<decltype(map_tail), const Map<int, int, 3>>));
    STATIC_REQUIRE(count(map_tail) == 2U);
    STATIC_REQUIRE(map_tail.begin()[0] == map.begin()[1]);
    STATIC_REQUIRE(map_tail.begin()[1] == map.begin()[2]);
    constexpr auto full_map = Map<int, int, 3>{MapEntry{1, 100}, MapEntry{2, 200}, MapEntry{3, 300}};
    constexpr auto full_map_tail = next(full_map);
    STATIC_REQUIRE((std::same_as<decltype(full_map_tail), const Map<int, int, 2>>));
    STATIC_REQUIRE(count(full_map_tail) == 2U);
    STATIC_REQUIRE(full_map_tail.begin()[0] == full_map.begin()[1]);
    STATIC_REQUIRE(full_map_tail.begin()[1] == full_map.begin()[2]);

    TRACE_ID("invariant.Next.MapSetOrderIsNotSemanticallySignificant");
    constexpr auto set = Set<int, 4>{10, 20, 30};
    constexpr auto set_tail = next(set);
    STATIC_REQUIRE((std::same_as<decltype(set_tail), const Set<int, 3>>));
    STATIC_REQUIRE(count(set_tail) == 2U);
    STATIC_REQUIRE(set_tail.begin()[0] == set.begin()[1]);
    STATIC_REQUIRE(set_tail.begin()[1] == set.begin()[2]);
    constexpr auto full_set = Set<int, 3>{10, 20, 30};
    constexpr auto full_set_tail = next(full_set);
    STATIC_REQUIRE((std::same_as<decltype(full_set_tail), const Set<int, 2>>));
    STATIC_REQUIRE(count(full_set_tail) == 2U);
    STATIC_REQUIRE(full_set_tail.begin()[0] == full_set.begin()[1]);
    STATIC_REQUIRE(full_set_tail.begin()[1] == full_set.begin()[2]);

    TRACE_ID("invariant.Next.QueuePreservesFifoOrder");
    STATIC_REQUIRE(queue_tail.begin()[0] == 20);
    STATIC_REQUIRE(queue_tail.begin()[1] == 30);

    TRACE_ID("invariant.Next.EmptyOrSingletonReturnsEmptyResult");
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
    STATIC_REQUIRE((std::same_as<decltype(next(Queue<int, 0>{})), Queue<int, 0>>));
    STATIC_REQUIRE(count(next(Queue<int, 0>{})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(next(Set<int, 0>{})), Set<int, 0>>));
    STATIC_REQUIRE(count(next(Set<int, 0>{})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(next(Map<int, int, 0>{})), Map<int, int, 0>>));
    STATIC_REQUIRE(count(next(Map<int, int, 0>{})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(next(String<0>{})), String<0>>));
    STATIC_REQUIRE(count(next(String<0>{})) == 0U);
    STATIC_REQUIRE((std::same_as<decltype(next(Queue<int, 1>{10})), Queue<int, 0>>));
    STATIC_REQUIRE(count(next(Queue<int, 1>{10})) == 0U);

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
    STATIC_REQUIRE(noexcept(next(cljonic::Range<int>{0, 5})));

    TRACE_ID("invariant.Next.RejectsUnsupportedDomainWithTargetedDiagnostic");
    TRACE_ID("invariant.Next.DiagnosticNamesSupportedDomains");
    STATIC_REQUIRE_FALSE(HasAdmittedNext<int>);
    STATIC_REQUIRE_FALSE(HasAdmittedNext<cljonic::MapEntry<int, int>>);
    STATIC_REQUIRE(HasAdmittedNext<cljonic::Range<int>>);
    STATIC_REQUIRE(HasAdmittedNext<Vector<int, 4>>);
    STATIC_REQUIRE(HasAdmittedNext<Map<int, int, 4>>);
    STATIC_REQUIRE(HasAdmittedNext<Set<int, 4>>);
    STATIC_REQUIRE(HasAdmittedNext<Queue<int, 4>>);
    STATIC_REQUIRE(HasAdmittedNext<String<4>>);
    STATIC_REQUIRE(HasAdmittedNext<Repeat<int>>);
    STATIC_REQUIRE(HasAdmittedNext<cljonic::Cycle<Vector<int, 3>>>);

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
    REQUIRE(runtime_queue_tail.begin()[0] == second);
    REQUIRE(runtime_queue_tail.begin()[1] == third);
    REQUIRE(count(runtime_queue) == 3U);

    const auto runtime_map = Map<int, int, 4>{MapEntry{first, second}, MapEntry{second, third}};
    const auto runtime_map_tail = next(runtime_map);
    REQUIRE(count(runtime_map_tail) == 1U);
    REQUIRE(runtime_map_tail.begin()[0] == runtime_map.begin()[1]);
    REQUIRE(count(runtime_map) == 2U);

    const auto runtime_set = Set<int, 4>{first, second, third};
    const auto runtime_set_tail = next(runtime_set);
    REQUIRE(count(runtime_set_tail) == 2U);
    REQUIRE(runtime_set_tail.begin()[0] == runtime_set.begin()[1]);
    REQUIRE(runtime_set_tail.begin()[1] == runtime_set.begin()[2]);
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

TEST_CASE("Next returns the logical tail of Repeat producers", "[next]") {
    using cljonic::count;
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::is_empty;
    using cljonic::next;
    using cljonic::Repeat;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Next");
    TRACE_ID("invariant.Next.SupportsProducerTailDomain");
    TRACE_ID("invariant.Next.ProducerTailDomainIncludesRange");
    TRACE_ID("invariant.Next.ReturnsSameProducerKindAsSource");
    TRACE_ID("invariant.Next.ReturnsElementsAfterFirstOfProducerResult");
    TRACE_ID("invariant.Next.UnboundedProducerTailRemainsUnbounded");
    TRACE_ID("invariant.Next.RepeatedNextNeverExhaustsUnboundedProducer");
    TRACE_ID("invariant.Next.ProducerTailResultIsSelfContainedAndOwning");
    TRACE_ID("invariant.Next.ProducerTailDoesNotMaterializeProducerResults");
    TRACE_ID("invariant.Next.ProducerTailCoherentWithCountIsFiniteIsEmptyIntoAndFitsInto");

    TRACE_ID("entity-fields.Repeat");
    TRACE_ID("invariant.Repeat.NextReturnsFiniteTailWithCountMinusOneSaturatingAtZero");
    TRACE_ID("invariant.Repeat.NextReturnsUnboundedTailWithUnchangedObservationPolicy");
    TRACE_ID("invariant.Repeat.NextTailPreservesInputAndRepeatedValue");
    TRACE_ID("invariant.Repeat.NextTailCoherentWithCountIsFiniteIsEmptyIntoAndFitsInto");

    constexpr auto finite = Repeat{3, 7};
    constexpr auto finite_tail = next(finite);
    STATIC_REQUIRE((std::same_as<decltype(finite_tail), const Repeat<int>>));
    STATIC_REQUIRE(count(finite_tail) == 2U);
    STATIC_REQUIRE(fits_into(Vector<int, 2>{}, finite_tail));
    constexpr auto finite_tail_values = into(Vector<int, 2>{}, finite_tail);
    STATIC_REQUIRE(finite_tail_values(0) == 7);
    STATIC_REQUIRE(finite_tail_values(1) == 7);
    STATIC_REQUIRE(count(finite) == 3U);
    STATIC_REQUIRE(fits_into(Vector<int, 3>{}, finite));
    constexpr auto finite_values = into(Vector<int, 3>{}, finite);
    STATIC_REQUIRE(finite_values(0) == 7);
    STATIC_REQUIRE(finite_values(1) == 7);
    STATIC_REQUIRE(finite_values(2) == 7);
    STATIC_REQUIRE(noexcept(next(finite)));

    constexpr auto singleton = Repeat{1, 7};
    constexpr auto singleton_tail = next(singleton);
    STATIC_REQUIRE(count(singleton_tail) == 0U);
    STATIC_REQUIRE(fits_into(Vector<int, 0>{}, singleton_tail));
    STATIC_REQUIRE(is_empty(singleton_tail));
    STATIC_REQUIRE(singleton_tail == Repeat{0, 7});

    constexpr auto empty = Repeat{0, 7};
    constexpr auto empty_tail = next(empty);
    STATIC_REQUIRE(count(empty_tail) == 0U);
    STATIC_REQUIRE(fits_into(Vector<int, 0>{}, empty_tail));
    STATIC_REQUIRE(empty_tail == empty);

    constexpr auto unbounded = Repeat{7};
    constexpr auto unbounded_tail = next(unbounded);
    STATIC_REQUIRE(count(unbounded_tail) == count(unbounded));
    STATIC_REQUIRE_FALSE(fits_into(Vector<int, 2>{}, unbounded_tail));
    constexpr auto unbounded_tail_values = into(Vector<int, 2>{}, unbounded_tail);
    STATIC_REQUIRE(unbounded_tail_values(0) == 7);
    STATIC_REQUIRE(unbounded_tail_values(1) == 7);
    constexpr auto repeated_tail = next(unbounded_tail);
    STATIC_REQUIRE(count(repeated_tail) == count(unbounded));
    STATIC_REQUIRE_FALSE(fits_into(Vector<int, 2>{}, repeated_tail));
    constexpr auto repeated_tail_values = into(Vector<int, 2>{}, repeated_tail);
    STATIC_REQUIRE(repeated_tail_values(0) == 7);
    STATIC_REQUIRE(repeated_tail_values(1) == 7);

    constexpr auto materialized_tail = into(Vector<int, 2>{}, finite_tail);
    STATIC_REQUIRE(count(materialized_tail) == 2U);
    STATIC_REQUIRE(materialized_tail(0) == 7);
    STATIC_REQUIRE(materialized_tail(1) == 7);
    STATIC_REQUIRE(fits_into(Vector<int, 2>{}, finite_tail));
    STATIC_REQUIRE_FALSE(fits_into(Vector<int, 2>{}, unbounded_tail));
    constexpr auto bounded_unbounded_tail = into(Vector<int, 2>{}, unbounded_tail);
    STATIC_REQUIRE(count(bounded_unbounded_tail) == 2U);
    STATIC_REQUIRE(bounded_unbounded_tail(0) == 7);
    STATIC_REQUIRE(bounded_unbounded_tail(1) == 7);
}

TEST_CASE("Next returns the logical tail of Range producers", "[next]") {
    using cljonic::count;
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::is_empty;
    using cljonic::next;
    using cljonic::Range;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Next");
    TRACE_ID("invariant.Next.SupportsProducerTailDomain");
    TRACE_ID("invariant.Next.ProducerTailDomainIncludesRange");
    TRACE_ID("invariant.Next.UnboundedProducerTailRemainsUnbounded");
    TRACE_ID("invariant.Next.RepeatedNextNeverExhaustsUnboundedProducer");
    TRACE_ID("invariant.Next.ReturnsSameProducerKindAsSource");
    TRACE_ID("invariant.Next.ProducerTailResultIsSelfContainedAndOwning");
    TRACE_ID("invariant.Next.ProducerTailDoesNotMaterializeProducerResults");
    TRACE_ID("invariant.Next.ProducerTailCoherentWithCountIsFiniteIsEmptyIntoAndFitsInto");
    TRACE_ID("invariant.Next.RangeTailPreservesEndpointAndStep");
    TRACE_ID("invariant.Next.ZeroStepRangeTailRemainsUnboundedWithSameObservationCap");
    TRACE_ID("invariant.Next.EmptyNonzeroStepRangeTailPreservesParameters");
    TRACE_ID("invariant.Next.SingletonNonzeroStepRangeTailIsEmptyAtEndpointWithoutStepAddition");
    TRACE_ID("invariant.Next.MultiElementNonzeroStepRangeTailAdvancesStartByStep");

    TRACE_ID("entity-fields.Range");
    TRACE_ID("invariant.Range.NextPreservesEndpointAndStep");
    TRACE_ID("invariant.Range.ZeroStepNextRemainsUnboundedWithSameObservationCap");
    TRACE_ID("invariant.Range.EmptyNonzeroStepNextPreservesParameters");
    TRACE_ID("invariant.Range.SingletonNonzeroStepNextIsEmptyAtEndpointWithoutStepAddition");
    TRACE_ID("invariant.Range.MultiElementNonzeroStepNextAdvancesStartByStep");

    constexpr auto ascending = Range{1, 8, 2};
    constexpr auto ascending_tail = next(ascending);
    STATIC_REQUIRE((std::same_as<decltype(ascending_tail), const Range<int>>));
    STATIC_REQUIRE(ascending_tail == Range{3, 8, 2});
    STATIC_REQUIRE(count(ascending_tail) == 3U);
    constexpr auto ascending_values = into(Vector<int, 3>{}, ascending_tail);
    STATIC_REQUIRE(ascending_values(0) == 3);
    STATIC_REQUIRE(ascending_values(1) == 5);
    STATIC_REQUIRE(ascending_values(2) == 7);
    STATIC_REQUIRE(ascending == Range{1, 8, 2});
    STATIC_REQUIRE(noexcept(next(ascending)));

    constexpr auto descending = Range{5, -6, -2};
    constexpr auto descending_tail = next(descending);
    STATIC_REQUIRE(descending_tail == Range{3, -6, -2});
    STATIC_REQUIRE(count(descending_tail) == 5U);
    constexpr auto descending_values = into(Vector<int, 2>{}, descending_tail);
    STATIC_REQUIRE(descending_values(0) == 3);
    STATIC_REQUIRE(descending_values(1) == 1);
    STATIC_REQUIRE(descending == Range{5, -6, -2});

    constexpr auto empty_away = Range{5, 0, 1};
    constexpr auto empty_away_tail = next(empty_away);
    STATIC_REQUIRE(empty_away_tail == empty_away);
    STATIC_REQUIRE(is_empty(empty_away_tail));
    constexpr auto empty_equal = Range{5, 5, -1};
    STATIC_REQUIRE(next(empty_equal) == empty_equal);

    constexpr auto singleton_overflow = Range{std::numeric_limits<int>::max() - 1, std::numeric_limits<int>::max(), 2};
    constexpr auto singleton_tail = next(singleton_overflow);
    STATIC_REQUIRE(singleton_tail == Range{std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 2});
    STATIC_REQUIRE(is_empty(singleton_tail));
    STATIC_REQUIRE(count(singleton_tail) == 0U);
    STATIC_REQUIRE(singleton_overflow ==
                   Range{std::numeric_limits<int>::max() - 1, std::numeric_limits<int>::max(), 2});

    constexpr auto zero_step = Range{4, 9, 0};
    constexpr auto zero_step_tail = next(zero_step);
    STATIC_REQUIRE(zero_step_tail == zero_step);
    STATIC_REQUIRE(count(zero_step_tail) == count(zero_step));
    STATIC_REQUIRE_FALSE(fits_into(Vector<int, 2>{}, zero_step_tail));
    constexpr auto repeated_zero_step_tail = next(zero_step_tail);
    STATIC_REQUIRE(repeated_zero_step_tail == zero_step);
    STATIC_REQUIRE(count(repeated_zero_step_tail) == count(zero_step));
    constexpr auto zero_step_values = into(Vector<int, 2>{}, zero_step_tail);
    STATIC_REQUIRE(zero_step_values(0) == 4);
    STATIC_REQUIRE(zero_step_values(1) == 4);

    volatile int start_raw = 5;
    const int start = start_raw;
    const auto runtime_ascending = Range{start, start + 7, 2};
    const auto runtime_ascending_tail = next(runtime_ascending);
    REQUIRE(runtime_ascending_tail == Range{start + 2, start + 7, 2});
    REQUIRE(count(runtime_ascending_tail) == 3U);
    REQUIRE(runtime_ascending == Range{start, start + 7, 2});

    const auto runtime_descending_tail = next(Range{start, start - 7, -2});
    REQUIRE(runtime_descending_tail == Range{start - 2, start - 7, -2});
    REQUIRE(count(runtime_descending_tail) == 3U);

    const auto runtime_empty = Range{start, start - 1, 1};
    REQUIRE(next(runtime_empty) == runtime_empty);
    REQUIRE(next(Range{start, start, -1}) == Range{start, start, -1});

    volatile int maximum_raw = std::numeric_limits<int>::max();
    const int maximum = maximum_raw;
    const auto runtime_singleton_tail = next(Range{maximum - 1, maximum, 2});
    REQUIRE(runtime_singleton_tail == Range{maximum, maximum, 2});
    REQUIRE(is_empty(runtime_singleton_tail));

    const auto runtime_zero_step = Range{start, start + 5, 0};
    REQUIRE(next(runtime_zero_step) == runtime_zero_step);
    REQUIRE(count(next(runtime_zero_step)) == count(runtime_zero_step));
}

TEST_CASE("Next returns the logical tail of Cycle producers", "[next]") {
    using cljonic::count;
    using cljonic::cycle;
    using cljonic::into;
    using cljonic::is_empty;
    using cljonic::iterate;
    using cljonic::next;
    using cljonic::Range;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Next");
    TRACE_ID("invariant.Next.SupportsProducerTailDomain");
    TRACE_ID("invariant.Next.ProducerTailDomainIncludesCycle");
    TRACE_ID("invariant.Next.ReturnsSameProducerKindAsSource");
    TRACE_ID("invariant.Next.UnboundedProducerTailRemainsUnbounded");
    TRACE_ID("invariant.Next.RepeatedNextNeverExhaustsUnboundedProducer");
    TRACE_ID("invariant.Next.ProducerTailResultIsSelfContainedAndOwning");
    TRACE_ID("invariant.Next.ProducerTailDoesNotMaterializeProducerResults");
    TRACE_ID("invariant.Next.ProducerTailCoherentWithCountIsFiniteIsEmptyIntoAndFitsInto");
    TRACE_ID("invariant.Next.FiniteCycleTailAdvancesStartingOffsetModuloObservedSourceCount");
    TRACE_ID("invariant.Next.EmptyFiniteCycleTailPreservesEmptyObservation");
    TRACE_ID("invariant.Next.UnboundedCycleTailAdvancesSourceAndResetsStartingOffset");

    TRACE_ID("entity-fields.Cycle");
    TRACE_ID("invariant.Cycle.NextPreservesCycleTypeAndObservationPolicy");
    TRACE_ID("invariant.Cycle.FiniteNonemptyNextAdvancesStartingOffsetModuloObservedSourceCount");
    TRACE_ID("invariant.Cycle.EmptyFiniteNextPreservesEmptyObservation");
    TRACE_ID("invariant.Cycle.UnboundedNextAdvancesOwnedSourceAndResetsStartingOffset");
    TRACE_ID("invariant.Cycle.NextSourceMustSupportNextOrBeCollection");
    TRACE_ID("invariant.Cycle.NextDoesNotMaterializeResults");
    TRACE_ID("invariant.Cycle.NextCallbackInvocationFollowsOwnedSourceNext");
    TRACE_ID("invariant.Next.NoHeapAllocation");
    TRACE_ID("invariant.Next.NoRtti");
    TRACE_ID("invariant.Next.NoExceptions");
    TRACE_ID("invariant.Next.ConstexprCapable");

    constexpr auto finite_cycle = cycle(Vector<int, 3>{1, 2, 3});
    constexpr auto finite_cycle_tail = next(finite_cycle);
    STATIC_REQUIRE((std::same_as<decltype(finite_cycle_tail), const decltype(finite_cycle)>));
    STATIC_REQUIRE(count(finite_cycle_tail) == count(finite_cycle));
    STATIC_REQUIRE(finite_cycle != finite_cycle_tail);
    STATIC_REQUIRE(noexcept(next(finite_cycle)));
    constexpr auto finite_cycle_tail_values = into(Vector<int, 5>{}, finite_cycle_tail);
    STATIC_REQUIRE(finite_cycle_tail_values(0) == 2);
    STATIC_REQUIRE(finite_cycle_tail_values(1) == 3);
    STATIC_REQUIRE(finite_cycle_tail_values(2) == 1);
    STATIC_REQUIRE(finite_cycle_tail_values(3) == 2);
    STATIC_REQUIRE(finite_cycle_tail_values(4) == 3);

    constexpr auto second_tail = next(finite_cycle_tail);
    constexpr auto second_tail_values = into(Vector<int, 5>{}, second_tail);
    STATIC_REQUIRE(second_tail_values(0) == 3);
    STATIC_REQUIRE(second_tail_values(1) == 1);
    STATIC_REQUIRE(second_tail_values(2) == 2);
    constexpr auto wrapped_tail = next(second_tail);
    STATIC_REQUIRE(wrapped_tail == finite_cycle);

    constexpr auto finite_range_cycle = cycle(Range{1, 4});
    constexpr auto finite_range_cycle_tail = next(finite_range_cycle);
    constexpr auto finite_range_cycle_values = into(Vector<int, 4>{}, finite_range_cycle_tail);
    STATIC_REQUIRE(finite_range_cycle_values(0) == 2);
    STATIC_REQUIRE(finite_range_cycle_values(1) == 3);
    STATIC_REQUIRE(finite_range_cycle_values(2) == 1);
    STATIC_REQUIRE(finite_range_cycle_values(3) == 2);

    constexpr auto empty_cycle = cycle(Vector<int, 0>{});
    constexpr auto empty_cycle_tail = next(empty_cycle);
    STATIC_REQUIRE(empty_cycle_tail == empty_cycle);
    STATIC_REQUIRE(is_empty(into(Vector<int, 2>{}, empty_cycle_tail)));

    constexpr auto unbounded_cycle = cycle(cycle(Vector<int, 3>{4, 5, 6}));
    constexpr auto unbounded_cycle_tail = next(unbounded_cycle);
    STATIC_REQUIRE(count(unbounded_cycle_tail) == count(unbounded_cycle));
    STATIC_REQUIRE(unbounded_cycle_tail != unbounded_cycle);
    constexpr auto unbounded_cycle_values = into(Vector<int, 5>{}, unbounded_cycle_tail);
    STATIC_REQUIRE(unbounded_cycle_values(0) == 5);
    STATIC_REQUIRE(unbounded_cycle_values(1) == 6);
    STATIC_REQUIRE(unbounded_cycle_values(2) == 4);
    STATIC_REQUIRE(unbounded_cycle_values(3) == 5);
    STATIC_REQUIRE(unbounded_cycle_values(4) == 6);
    constexpr auto repeated_unbounded_cycle_tail = next(unbounded_cycle_tail);
    STATIC_REQUIRE(count(repeated_unbounded_cycle_tail) == count(unbounded_cycle));
    constexpr auto repeated_unbounded_values = into(Vector<int, 5>{}, repeated_unbounded_cycle_tail);
    STATIC_REQUIRE(repeated_unbounded_values(0) == 6);
    STATIC_REQUIRE(repeated_unbounded_values(1) == 4);

    constexpr auto iterate_cycle = cycle(iterate([](int value) noexcept { return value + 1; }, 0));
    STATIC_REQUIRE(HasAdmittedNext<std::remove_cvref_t<decltype(iterate_cycle)>>);
    constexpr auto iterate_cycle_tail = next(iterate_cycle);
    constexpr auto iterate_cycle_values = into(Vector<int, 3>{}, iterate_cycle_tail);
    STATIC_REQUIRE(iterate_cycle_values(0) == 1);
    STATIC_REQUIRE(iterate_cycle_values(1) == 2);
    STATIC_REQUIRE(iterate_cycle_values(2) == 3);

    volatile int first_raw = 10;
    const int first = first_raw;
    const auto runtime_cycle = cycle(Vector<int, 3>{first, first + 1, first + 2});
    const auto runtime_tail = next(runtime_cycle);
    const auto runtime_values = into(Vector<int, 4>{}, runtime_tail);
    REQUIRE(runtime_values(0) == first + 1);
    REQUIRE(runtime_values(1) == first + 2);
    REQUIRE(runtime_values(2) == first);
    REQUIRE(runtime_values(3) == first + 1);
    REQUIRE(next(next(runtime_tail)) == runtime_cycle);
    REQUIRE(into(Vector<int, 1>{}, runtime_cycle)(0) == first);

    const auto runtime_range_cycle = cycle(Range{first, first + 3});
    const auto runtime_range_tail = next(runtime_range_cycle);
    const auto runtime_range_values = into(Vector<int, 4>{}, runtime_range_tail);
    REQUIRE(runtime_range_values(0) == first + 1);
    REQUIRE(runtime_range_values(1) == first + 2);
    REQUIRE(runtime_range_values(2) == first);
    REQUIRE(runtime_range_values(3) == first + 1);

    const auto runtime_empty_cycle = cycle(Range{first, first});
    const auto runtime_empty_tail = next(runtime_empty_cycle);
    REQUIRE(runtime_empty_tail == runtime_empty_cycle);
    REQUIRE(is_empty(into(Vector<int, 2>{}, runtime_empty_tail)));
}

TEST_CASE("Next eagerly advances callback-backed producers once", "[next]") {
    using cljonic::count;
    using cljonic::cycle;
    using cljonic::into;
    using cljonic::iterate;
    using cljonic::next;
    using cljonic::repeatedly;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Next");
    TRACE_ID("invariant.Next.ProducerTailDomainIncludesIterate");
    TRACE_ID("invariant.Next.ProducerTailDomainIncludesRepeatedly");
    TRACE_ID("invariant.Next.CallbackProducerTailInvokesOneCallbackWhenNonempty");
    TRACE_ID("invariant.Next.IterateTailUsesAdvancedValueAsInitial");
    TRACE_ID("invariant.Next.RepeatedlyTailDiscardsOneValueAndPreservesAdvancedStepState");
    TRACE_ID("invariant.Next.FiniteRepeatedlyTailDecrementsCount");
    TRACE_ID("invariant.Next.UnboundedCallbackProducerTailPreservesObservationCap");
    TRACE_ID("invariant.Next.EmptyCallbackProducerTailDoesNotInvokeCallback");
    TRACE_ID("invariant.Next.ProducerTailDoesNotMaterializeProducerResults");
    TRACE_ID("invariant.Next.ProducerTailCoherentWithCountIsFiniteIsEmptyIntoAndFitsInto");

    TRACE_ID("entity-fields.Iterate");
    TRACE_ID("invariant.Iterate.NextInvokesStepOnceWhenObservationCapIsNonzero");
    TRACE_ID("invariant.Iterate.NextTailUsesStepResultAsNewInitialValue");
    TRACE_ID("invariant.Iterate.NextPreservesObservationCap");
    TRACE_ID("invariant.Iterate.NextZeroObservationCapDoesNotInvokeStep");

    constexpr auto iteration = iterate([](int value) noexcept { return value + 1; }, 10);
    constexpr auto iteration_tail = next(iteration);
    STATIC_REQUIRE(count(iteration_tail) == count(iteration));
    constexpr auto iteration_values = into(Vector<int, 3>{}, iteration_tail);
    STATIC_REQUIRE(iteration_values(0) == 11);
    STATIC_REQUIRE(iteration_values(1) == 12);
    STATIC_REQUIRE(iteration_values(2) == 13);
    constexpr auto original_iteration_values = into(Vector<int, 3>{}, iteration);
    STATIC_REQUIRE(original_iteration_values(0) == 10);
    STATIC_REQUIRE(original_iteration_values(1) == 11);

    TRACE_ID("entity-fields.Repeatedly");
    TRACE_ID("invariant.Repeatedly.NextInvokesStepOnceWhenCountIsPositive");
    TRACE_ID("invariant.Repeatedly.NextDiscardsOneGeneratedElement");
    TRACE_ID("invariant.Repeatedly.NextTailStoresAdvancedStepCopy");
    TRACE_ID("invariant.Repeatedly.NextFiniteTailDecrementsCount");
    TRACE_ID("invariant.Repeatedly.NextUnboundedTailPreservesObservationCap");
    TRACE_ID("invariant.Repeatedly.NextZeroCountDoesNotInvokeStep");
    TRACE_ID("invariant.Repeatedly.NextAdvancesValueHeldStateOnlyInTail");
    TRACE_ID("invariant.Repeatedly.NextPreservesSharedReferenceOrStaticEffects");

    constexpr auto repeated = repeatedly(3, [value = 0]() mutable noexcept { return ++value; });
    constexpr auto repeated_tail = next(repeated);
    STATIC_REQUIRE(count(repeated_tail) == 2U);
    constexpr auto repeated_tail_values = into(Vector<int, 3>{}, repeated_tail);
    STATIC_REQUIRE(repeated_tail_values(0) == 2);
    STATIC_REQUIRE(repeated_tail_values(1) == 3);
    constexpr auto original_repeated_values = into(Vector<int, 3>{}, repeated);
    STATIC_REQUIRE(original_repeated_values(0) == 1);
    STATIC_REQUIRE(original_repeated_values(1) == 2);
    STATIC_REQUIRE(original_repeated_values(2) == 3);

    auto callback_calls = 0;
    const auto effectful = repeatedly(3, [&callback_calls]() noexcept { return ++callback_calls; });
    const auto effectful_tail = next(effectful);
    REQUIRE(callback_calls == 1);
    REQUIRE(count(effectful_tail) == 2U);

    const auto empty_effectful = repeatedly(0, [&callback_calls]() noexcept { return ++callback_calls; });
    const auto empty_effectful_tail = next(empty_effectful);
    REQUIRE(callback_calls == 1);
    REQUIRE(count(empty_effectful_tail) == 0U);

    const auto unbounded_effectful = repeatedly([&callback_calls]() noexcept { return ++callback_calls; });
    const auto unbounded_effectful_tail = next(unbounded_effectful);
    REQUIRE(callback_calls == 2);
    REQUIRE(count(unbounded_effectful_tail) == count(unbounded_effectful));
    REQUIRE_FALSE(cljonic::fits_into(Vector<int, 2>{}, unbounded_effectful_tail));

    TRACE_ID("entity-fields.Cycle");
    TRACE_ID("invariant.Cycle.NextCallbackInvocationFollowsOwnedSourceNext");
    auto cycle_callback_calls = 0;
    const auto effectful_cycle =
        cycle(repeatedly([&cycle_callback_calls]() noexcept { return ++cycle_callback_calls; }));
    const auto effectful_cycle_tail = next(effectful_cycle);
    REQUIRE(cycle_callback_calls == 1);
    REQUIRE(count(effectful_cycle_tail) == count(effectful_cycle));
}
