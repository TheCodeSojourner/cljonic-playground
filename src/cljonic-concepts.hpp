#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

namespace cljonic::concepts {

/** Requires that \p value_type is default-initializable and copyable - the
 * minimum contract for current Vector element storage semantics.
 */
template <typename value_type>
concept VectorElement =
    std::default_initializable<value_type> && std::copyable<value_type>;

/** Detects whether copy-assignment of \p value_type is \c noexcept.
 */
/** Requires that Vector storage can be initialized and assigned without
 * throwing.
 */
template <typename value_type>
concept NothrowVectorElement =
    VectorElement<value_type> &&
    std::is_nothrow_default_constructible_v<value_type> &&
    std::is_nothrow_copy_assignable_v<value_type>;

/** Requires that an argument is implicitly convertible to and can construct a
 * Vector element without throwing.
 */
template <typename value_type, typename argument_type>
concept NothrowElementConstruction =
    std::convertible_to<argument_type, value_type> &&
    std::is_nothrow_constructible_v<value_type, argument_type>;

/** Requires that \p collection_type exposes a \c size() member returning a
 * count of logical elements.
 */
template <typename collection_type>
concept Collection = requires(const collection_type &c) {
  { c.size() } -> std::convertible_to<std::size_t>;
};

} // namespace cljonic::concepts
