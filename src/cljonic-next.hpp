#pragma once

#include <cljonic-concepts.hpp>
#include <cljonic-map.hpp>
#include <cljonic-queue.hpp>
#include <cljonic-set.hpp>
#include <cljonic-string.hpp>
#include <cljonic-vector.hpp>

#include <cstddef>

namespace cljonic {

namespace concepts_detail {

template <typename C>
concept NextSequenceableCollection =
    concepts::SequenceableCollection<C> && concepts::NothrowConstInputRange<C> &&
    (concepts::CljonicVector<C> || concepts::CljonicMap<C> || concepts::CljonicSet<C> || concepts::CljonicQueue<C> ||
     concepts::CljonicString<C>);

constexpr auto next_tail_capacity(const std::size_t capacity) noexcept -> std::size_t {
    return capacity == 0U ? 0U : capacity - 1U;
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
 * Queue "Queue", or \ref String "String" as a value of the same collection kind. The input remains unchanged.
 *
 * The result has a static capacity of one less than the source (zero when the source capacity is zero) and contains
 * every logical element after the first in the source's traversal order. An empty or singleton input returns an empty
 * value of the result type.
 *
 * A map's elements remain value-semantic \ref MapEntryFactory "map_entry" values. Map and Set traversal order is not
 * semantically significant, while Queue retains FIFO order.
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
           runtime_text(1) == 'E')
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

template <typename C>
    requires(!concepts_detail::NextSequenceableCollection<C>)
constexpr auto next([[maybe_unused]] const C& value) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::next: expected a Vector, Map, Set, Queue, or String value.");
}

} // namespace cljonic
