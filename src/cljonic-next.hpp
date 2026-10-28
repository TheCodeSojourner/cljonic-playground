#pragma once

#include <cljonic-concepts.hpp>
#include <cljonic-cycle.hpp>
#include <cljonic-iterate.hpp>
#include <cljonic-map.hpp>
#include <cljonic-queue.hpp>
#include <cljonic-range.hpp>
#include <cljonic-repeat.hpp>
#include <cljonic-repeatedly.hpp>
#include <cljonic-set.hpp>
#include <cljonic-string.hpp>
#include <cljonic-vector.hpp>

#include <cstddef>
#include <functional>
#include <type_traits>

namespace cljonic {

namespace concepts_detail {

template <typename C>
concept NextSequenceableCollection =
    concepts::SequenceableCollection<C> && concepts::NothrowConstInputRange<C> &&
    (concepts::CljonicVector<C> || concepts::CljonicMap<C> || concepts::CljonicSet<C> || concepts::CljonicQueue<C> ||
     concepts::CljonicString<C>);

template <typename P>
struct NextProducerDomain : std::false_type {};

template <typename T>
struct NextProducerDomain<Range<T>> : std::true_type {};

template <typename T>
struct NextProducerDomain<Repeat<T>> : std::true_type {};

template <typename T, typename Step>
struct NextProducerDomain<Iterate<T, Step>> : std::true_type {};

template <typename T, typename Step>
struct NextProducerDomain<Repeatedly<T, Step>> : std::true_type {};

template <typename Source>
struct NextProducerDomain<Cycle<Source>>
    : std::bool_constant<NextSequenceableCollection<Source> || NextProducerDomain<Source>::value> {};

template <typename P>
concept NextSequenceableProducer =
    concepts::SequenceableProducer<P> && concepts::NothrowConstInputRange<P> && NextProducerDomain<P>::value;

template <typename T>
concept NextSequenceable = NextSequenceableCollection<T> || NextSequenceableProducer<T>;

constexpr auto next_tail_capacity(const std::size_t capacity) noexcept -> std::size_t {
    return capacity == 0U ? 0U : capacity - 1U;
}

struct RepeatNextAccess {
    template <concepts::CljonicRepeat P>
    [[nodiscard]] static constexpr auto tail(const P& producer) noexcept -> P {
        if (producer.is_finite_) {
            const auto remaining_count = producer.count_ == 0U ? 0U : producer.count_ - 1U;
            return P{remaining_count, producer.value_};
        }
        return P{producer.value_};
    }
};

struct RangeNextAccess {
    template <concepts::CljonicRange P>
    [[nodiscard]] static constexpr auto tail(const P& producer) noexcept -> P {
        if (producer.step_ == 0) {
            return producer;
        }

        const auto extent = producer.nonzero_step_extent();
        if (extent == 0U) {
            return producer;
        }
        if (extent == 1U) {
            return P{producer.end_, producer.end_, producer.step_};
        }
        return P{
            static_cast<decltype(producer.start_)>(producer.start_ + producer.step_),
            producer.end_,
            producer.step_,
        };
    }
};

struct CycleNextAccess {
    template <concepts::CljonicCycle P>
    [[nodiscard]] static constexpr auto finite_tail(const P& producer) noexcept -> P {
        const auto source_count = producer.source_.count();
        if (source_count == 0U) {
            return producer;
        }
        const auto next_offset = producer.starting_offset_ == source_count - 1U ? 0U : producer.starting_offset_ + 1U;
        return P{producer.source_, next_offset};
    }

    template <concepts::CljonicCycle P>
    [[nodiscard]] static constexpr auto tail(const P& producer) noexcept -> P;
};

struct IterateNextAccess {
    template <concepts::CljonicIterate P>
    [[nodiscard]] static constexpr auto tail(const P& producer) noexcept -> P {
        if (producer.count() == 0U) {
            return producer;
        }
        return P{std::invoke(producer.step_, producer.initial_), producer.step_};
    }
};

struct RepeatedlyNextAccess {
    template <concepts::CljonicRepeatedly P>
    [[nodiscard]] static constexpr auto tail(const P& producer) noexcept -> P {
        if (producer.count() == 0U) {
            return producer;
        }

        auto step = producer.step_;
        [[maybe_unused]] const auto discarded = std::invoke(step);
        if (producer.is_finite_) {
            return P{step, producer.count_ - 1U};
        }
        return P{step};
    }
};

template <concepts::CljonicRange P>
[[nodiscard]] constexpr auto next_producer_tail(const P& producer) noexcept -> P {
    return RangeNextAccess::tail(producer);
}

template <concepts::CljonicRepeat P>
[[nodiscard]] constexpr auto next_producer_tail(const P& producer) noexcept -> P {
    return RepeatNextAccess::tail(producer);
}

template <concepts::CljonicIterate P>
[[nodiscard]] constexpr auto next_producer_tail(const P& producer) noexcept -> P {
    return IterateNextAccess::tail(producer);
}

template <concepts::CljonicRepeatedly P>
[[nodiscard]] constexpr auto next_producer_tail(const P& producer) noexcept -> P {
    return RepeatedlyNextAccess::tail(producer);
}

template <concepts::CljonicCycle P>
[[nodiscard]] constexpr auto next_producer_tail(const P& producer) noexcept -> P {
    return CycleNextAccess::tail(producer);
}

template <typename C>
struct NextResult;

template <typename T, std::size_t N>
struct NextResult<Vector<T, N>> {
    using type = Vector<T, next_tail_capacity(N)>;
};

template <typename K, typename V, std::size_t N>
struct NextResult<Map<K, V, N>> {
    using type = Map<K, V, next_tail_capacity(N)>;
};

template <typename T, std::size_t N>
struct NextResult<Set<T, N>> {
    using type = Set<T, next_tail_capacity(N)>;
};

template <typename T, std::size_t N>
struct NextResult<Queue<T, N>> {
    using type = Queue<T, next_tail_capacity(N)>;
};

template <std::size_t N>
struct NextResult<String<N>> {
    using type = String<next_tail_capacity(N)>;
};

} // namespace concepts_detail

/** \anchor Next
 * The \b next free function returns the logical tail of a \ref Vector "Vector", \ref Map "Map", \ref Set "Set", \ref
 * Queue "Queue", or \ref String "String" as a value of the same collection kind, or of a supported producer as an
 * owning value of the same producer kind. The input remains unchanged.
 *
 * The result has a static capacity of one less than the source (zero when the source capacity is zero) and contains
 * every logical element after the first in the source's traversal order. An empty or singleton input returns an empty
 * value of the result type.
 *
 * A map's elements remain value-semantic \ref MapEntryFactory "map_entry" values. Map and Set traversal order is not
 * semantically significant, while Queue retains FIFO order.
 *
 * For a finite counted \ref Repeat "Repeat", the result is finite with the same repeated value and one fewer count,
 * saturating at zero. For an unbounded Repeat, the result remains unbounded with its observation policy unchanged.
 *
 * For a \ref Range "Range", the result preserves its endpoint and step. A zero-step Range remains unbounded, an empty
 * finite Range is unchanged, and a singleton finite Range becomes empty at its endpoint without adding the step.
 * Otherwise, the start advances by one step.
 *
 * For a \ref Cycle "Cycle" with a finite source, the starting position advances by one modulo the source's observed
 * count; an empty finite source remains empty. For a Cycle with an unbounded source, `next` tails its owned source and
 * resets the starting position, preserving the observation cap and following the source's callback behavior.
 *
 * For an \ref Iterate "Iterate", `next` invokes the step once and uses its result as the returned producer's initial
 * value while preserving the observation cap. A zero observation cap does not invoke the step.
 *
 * For a \ref Repeatedly "Repeatedly", `next` invokes a copy of the step once, discards that produced value, and stores
 * the advanced step copy in the tail. A finite Repeatedly tail has one fewer element; an unbounded Repeatedly keeps
 * its observation cap. An empty Repeatedly does not invoke its step. Value-held mutable state advances in the returned
 * Repeatedly only, while effects through referenced or static state occur when `next` is called and remain shared.
 *
 * Calling `next` does not materialize the remaining producer sequence into a collection.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto values = vector(10, 20, 30);
   constexpr auto vector_tail = next(values);
   static_assert(count(vector_tail) == 2);
   static_assert(vector_tail(0) == 20 && vector_tail(1) == 30);

   constexpr auto map = hash_map(1, 100, 2, 200);
   constexpr auto map_tail = next(map);
   static_assert(count(map_tail) == 1);
   // The entry dropped by next is the first one in traversal order.
   constexpr auto dropped_key = first(first(map));
   static_assert(!contains(map_tail, dropped_key));

   constexpr auto set = hash_set(10, 20, 30);
   constexpr auto set_tail = next(set);
   static_assert(count(set_tail) == 2);
   static_assert(!contains(set_tail, first(set)));

   constexpr auto q = queue(10, 20, 30);
   constexpr auto q_tail = next(q);
   static_assert(first(q_tail) == 20 && first(next(q_tail)) == 30);

   constexpr auto text = string("ABC");
   constexpr auto text_tail = next(text);
   static_assert(text_tail(0) == 'B' && text_tail(1) == 'C');
   static_assert(count(next(vector<int, 3>())) == 0);
   static_assert(count(next(vector(10))) == 0);

   constexpr auto repeated_tail = next(repeat(3, 7));
   constexpr auto repeated_tail_values = into(Vector<int, 2>{}, repeated_tail);
   static_assert(count(repeated_tail) == 2);
   static_assert(repeated_tail_values(0) == 7 && repeated_tail_values(1) == 7);
   constexpr auto unbounded_tail = next(repeat(7));
   static_assert(!fits_into(Vector<int, 2>{}, unbounded_tail));
   static_assert(count(unbounded_tail) == count(repeat(7)));
   constexpr auto unbounded_tail_values = into(Vector<int, 2>{}, unbounded_tail);
   static_assert(unbounded_tail_values(0) == 7 && unbounded_tail_values(1) == 7);

   constexpr auto range_tail = next(Range{1, 8, 2});
   constexpr auto range_tail_values = into(Vector<int, 3>{}, range_tail);
   static_assert(count(range_tail) == 3);
   static_assert(range_tail_values(0) == 3 && range_tail_values(1) == 5 &&
                 range_tail_values(2) == 7);
   constexpr auto unbounded_range_tail = next(Range{4, 9, 0});
   static_assert(!fits_into(Vector<int, 2>{}, unbounded_range_tail));
   static_assert(count(unbounded_range_tail) == count(Range{4, 9, 0}));

   constexpr auto cycle_tail = next(cycle(vector(1, 2, 3)));
   constexpr auto cycle_tail_values = into(Vector<int, 5>{}, cycle_tail);
   static_assert(cycle_tail_values(0) == 2 && cycle_tail_values(1) == 3 &&
                 cycle_tail_values(2) == 1 && cycle_tail_values(3) == 2 &&
                 cycle_tail_values(4) == 3);

   // Runtime demonstration.
   const auto runtime_values = vector<int, 4>(40, 50);
   const auto runtime_tail = next(runtime_values);
   const auto runtime_queue = queue<int, 4>(60, 70);
   const auto runtime_queue_tail = next(runtime_queue);
   const auto runtime_map = hash_map<int, int, 4>(3, 300, 4, 400);
   const auto runtime_map_tail = next(runtime_map);
   const auto runtime_set = hash_set<int, 4>(80, 90);
   const auto runtime_set_tail = next(runtime_set);
   const auto runtime_text = string<4>("DE");
   const auto runtime_text_tail = next(runtime_text);
   const auto runtime_repeat_tail = next(repeat(3, 7));
   const auto runtime_repeat_tail_values =
       into(Vector<int, 2>{}, runtime_repeat_tail);
   const auto runtime_unbounded_repeat_tail = next(repeat(7));
   const auto runtime_unbounded_repeat_tail_values =
       into(Vector<int, 2>{}, runtime_unbounded_repeat_tail);
   const auto runtime_range_tail = next(Range{5, -6, -2});
   const auto runtime_range_tail_values =
       into(Vector<int, 2>{}, runtime_range_tail);
   const auto runtime_unbounded_range_tail = next(Range{4, 9, 0});
   const auto runtime_cycle_tail = next(cycle(vector(1, 2, 3)));
   const auto runtime_cycle_tail_values =
       into(Vector<int, 5>{}, runtime_cycle_tail);
   const auto runtime_unbounded_cycle_tail = next(cycle(Range{4, 9, 0}));
   auto callback_calls = 0;
   const auto callback_producer =
       repeatedly(2, [&callback_calls]() noexcept { return ++callback_calls; });
   const auto callback_tail = next(callback_producer);

   return (count(runtime_tail) == 1 && runtime_tail(0) == 50 &&
           count(runtime_values) == 2 && runtime_values(0) == 40 &&
           runtime_values(1) == 50 && first(runtime_queue_tail) == 70 &&
           count(runtime_queue) == 2 && peek(runtime_queue) == 60 &&
           first(next(runtime_queue)) == 70 && count(runtime_map_tail) == 1 &&
           !contains(runtime_map_tail, first(first(runtime_map))) &&
           count(runtime_map) == 2 && get(runtime_map, 3) == 300 &&
           get(runtime_map, 4) == 400 && count(runtime_set_tail) == 1 &&
           !contains(runtime_set_tail, first(runtime_set)) &&
           count(runtime_set) == 2 && contains(runtime_set, 80) &&
           contains(runtime_set, 90) && runtime_text_tail(0) == 'E' &&
           count(runtime_text) == 2 && runtime_text(0) == 'D' &&
           runtime_text(1) == 'E' && count(runtime_repeat_tail) == 2 &&
           runtime_repeat_tail_values(0) == 7 &&
           runtime_repeat_tail_values(1) == 7 &&
           fits_into(Vector<int, 2>{}, runtime_repeat_tail) &&
           count(runtime_unbounded_repeat_tail) == count(repeat(7)) &&
           !fits_into(Vector<int, 2>{}, runtime_unbounded_repeat_tail) &&
           runtime_unbounded_repeat_tail_values(0) == 7 &&
           runtime_unbounded_repeat_tail_values(1) == 7 &&
           count(runtime_range_tail) == 5 && runtime_range_tail_values(0) == 3 &&
           runtime_range_tail_values(1) == 1 &&
           count(runtime_unbounded_range_tail) == count(Range{4, 9, 0}) &&
           !fits_into(Vector<int, 2>{}, runtime_unbounded_range_tail) &&
           runtime_cycle_tail_values(0) == 2 &&
           runtime_cycle_tail_values(1) == 3 &&
           runtime_cycle_tail_values(2) == 1 &&
           runtime_cycle_tail_values(3) == 2 &&
           runtime_cycle_tail_values(4) == 3 &&
           !fits_into(Vector<int, 2>{}, runtime_cycle_tail) &&
           count(runtime_unbounded_cycle_tail) == count(cycle(Range{4, 9, 0})) &&
           !fits_into(Vector<int, 2>{}, runtime_unbounded_cycle_tail) &&
           callback_calls == 1 && count(callback_tail) == 1)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts_detail::NextSequenceableCollection C>
[[nodiscard]] constexpr auto next(const C& collection) noexcept -> concepts_detail::NextResult<C>::type {
    using Result = concepts_detail::NextResult<C>::type;
    Result result{};
    auto iterator = collection.begin();
    const auto end = collection.end();
    if (iterator != end) {
        ++iterator;
    }
    for (; iterator != end; ++iterator) {
        result = result.conj(*iterator);
    }
    return result;
}

template <concepts_detail::NextSequenceableProducer P>
[[nodiscard]] constexpr auto next(const P& producer) noexcept -> P {
    return concepts_detail::next_producer_tail(producer);
}

template <concepts::CljonicCycle P>
[[nodiscard]] constexpr auto concepts_detail::CycleNextAccess::tail(const P& producer) noexcept -> P {
    if constexpr (concepts::CljonicCollection<typename P::source_type>) {
        return finite_tail(producer);
    } else {
        if (producer.source_finiteness()) {
            return finite_tail(producer);
        }
        return P{next(producer.source_), 0U};
    }
}

template <typename C>
    requires(!concepts_detail::NextSequenceableCollection<C> && !concepts_detail::NextSequenceableProducer<C>)
constexpr auto next([[maybe_unused]] const C& value) noexcept -> void {
    static_assert(
        concepts_detail::dependent_false<C>,
        "cljonic::next: expected a Vector, Map, Set, Queue, String, Range, Repeat, Iterate, Repeatedly, or supported "
        "Cycle value.");
}

} // namespace cljonic
