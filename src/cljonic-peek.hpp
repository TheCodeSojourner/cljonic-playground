#ifndef CLJONIC_PEEK_HPP
#define CLJONIC_PEEK_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic
{

    /** \anchor Peek
     * \brief Observes the accessible element without removal.
     *
     * \b Examples
     * ~~~~~{.cpp}
     * #include <cljonic.hpp>
     *
     * int main() {
     *   using namespace cljonic;
     *
     *   // Compile-time demonstration.
     *   constexpr auto q_const = conj(Queue<int, 4>{}, 99);
     *   static_assert(peek(q_const) == 99);
     *
     *   // Runtime demonstration.
     *   auto q_runtime = conj(Queue<int, 4>{}, 77);
     *   const auto v = peek(q_runtime);
     *
     *   return (v == 77) ? 0 : 1;
     * }
     * ~~~~~
     */
    template <typename C>
    [[nodiscard]] constexpr auto peek(const C &collection) noexcept
    {
        return collection.peek();
    }

} // namespace cljonic

#endif // CLJONIC_PEEK_HPP
