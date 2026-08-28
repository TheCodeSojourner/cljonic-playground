#ifndef CLJONIC_FIRST_HPP
#define CLJONIC_FIRST_HPP

#include <cljonic-concepts.hpp>
#include <cljonic-map.hpp>

namespace cljonic
{

    /** \anchor First
     * \brief Returns the first element of a sequenceable collection, or the key of
     *        a MapEntry.
     *
     * For a MapEntry value, `first` returns its key. For a sequenceable collection,
     * `first` returns the element at index zero.
     *
     * \b Examples
     * ~~~~~{.cpp}
     * #include <cljonic.hpp>
     *
     * int main() {
     *   using namespace cljonic;
     *
     *   // Compile-time demonstration.
     *   constexpr auto v_const = Vector<int, 4>{10, 20, 30};
     *   constexpr MapEntry<int, int> e_const{1, 100};
     *   static_assert(first(v_const) == 10);
     *   static_assert(first(e_const) == 1);
     *
     *   // Runtime demonstration.
     *   auto v_runtime = Vector<int, 4>{100, 200};
     *   const auto fst = first(v_runtime);
     *
     *   return (fst == 100) ? 0 : 1;
     * }
     * ~~~~~
     */
    template <typename KeyType, typename ValueType>
    [[nodiscard]] constexpr auto
    first(const MapEntry<KeyType, ValueType> &entry) noexcept -> const KeyType &
    {
        return entry.key;
    }

    /** Returns the element at index zero of the sequenceable collection. */
    template <typename C>
    [[nodiscard]] constexpr auto
    first(const C &collection) noexcept -> decltype(collection(0U))
    {
        return collection(0U);
    }

} // namespace cljonic

#endif // CLJONIC_FIRST_HPP