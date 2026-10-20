#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

namespace {
template <typename C>
auto owns_preserved_value(C input) noexcept -> bool {
    const auto original = input;
    auto result = not_empty(input);
    if (!equal(result, original) || !equal(input, original)) {
        return false;
    }
    input = C{};
    if (!equal(result, original)) {
        return false;
    }
    result = C{};
    return is_empty(result) && equal(not_empty(original), original);
}
} // namespace

[[nodiscard]] auto not_empty_probe() noexcept -> bool {
    volatile int raw = 7;
    const int value = raw;
    volatile char raw_character = 'x';
    const char character = raw_character;
    const char characters[]{character, 'y', '\0'};
    const char full_characters[]{character, '\0'};
    const bool nonempty =
        owns_preserved_value(Vector<int, 4>{value, 2}) &&
        owns_preserved_value(Map<int, int, 4>{MapEntry{1, value}, MapEntry{2, 20}}) &&
        owns_preserved_value(Set<int, 4>{value, 2}) && owns_preserved_value(Queue<int, 4>{value, 2}) &&
        owns_preserved_value(String<4>{characters}) && owns_preserved_value(Vector{Vector{value, 2}, Vector{3, 4}}) &&
        owns_preserved_value(Queue<int, 3>{1, 2, 3}.pop().conj(value));
    const bool empty = owns_preserved_value(Vector<int, 4>{}) && owns_preserved_value(Map<int, int, 4>{}) &&
                       owns_preserved_value(Set<int, 4>{}) && owns_preserved_value(Queue<int, 4>{}) &&
                       owns_preserved_value(String<4>{});
    const bool zero_capacity = owns_preserved_value(Vector<int, 0>{}) && owns_preserved_value(Map<int, int, 0>{}) &&
                               owns_preserved_value(Set<int, 0>{}) && owns_preserved_value(Queue<int, 0>{}) &&
                               owns_preserved_value(String<0>{});
    const bool full = owns_preserved_value(Vector<int, 1>{value}) &&
                      owns_preserved_value(Map<int, int, 1>{MapEntry{1, value}}) &&
                      owns_preserved_value(Set<int, 1>{value}) && owns_preserved_value(Queue<int, 1>{value}) &&
                      owns_preserved_value(String<1>{full_characters});
    const int default_value = value - value;
    const bool default_elements = owns_preserved_value(Vector{default_value}) &&
                                  owns_preserved_value(Map{MapEntry{default_value, default_value}}) &&
                                  owns_preserved_value(Set{default_value}) &&
                                  owns_preserved_value(Queue{default_value});
    return nonempty && empty && zero_capacity && full && default_elements;
}

} // namespace cljonic::no_heap::probes
