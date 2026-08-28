#ifndef CLJONIC_DISJ_HPP
#define CLJONIC_DISJ_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic
{

    /** \anchor Disj
     * \brief Disjoins an element from a set.
     *
     * \b Examples
     * ~~~~~{.cpp}
     * #include <cljonic.hpp>
     *
     * int main() {
     *   using namespace cljonic;
     *
     *   // Compile-time demonstration.
     *   constexpr auto s0_const = conj(Set<int, 4>{}, 42);
     *   constexpr auto s1_const = disj(s0_const, 42);
     *   static_assert(!s1_const.contains(42));
     *
     *   // Runtime demonstration.
     *   auto s0_runtime = conj(Set<int, 4>{}, 99);
     *   auto s1_runtime = disj(s0_runtime, 99);
     *
     *   return (!s1_runtime.contains(99) && s1_runtime.empty()) ? 0 : 1;
     * }
     * ~~~~~
     */
    template <typename C, typename T>
    [[nodiscard]] constexpr auto disj(const C &collection,
                                      const T &value) noexcept
    {
        return collection.disj(value);
    }

} // namespace cljonic

#endif // CLJONIC_DISJ_HPP
