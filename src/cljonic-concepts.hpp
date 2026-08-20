#pragma once

#include <concepts>
#include <cstddef>

namespace cljonic::concepts {

/** Requires that \p value_type is default-initializable and copyable - the
 * minimum contract for current Vector element storage semantics.
 */
template <typename value_type>
concept VectorElement =
    std::default_initializable<value_type> && std::copyable<value_type>;

/** Requires that Vector storage can be initialized and assigned without
 * throwing.
 */
template <typename value_type>
concept NothrowVectorElement =
    VectorElement<value_type> &&
    requires(value_type value, const value_type &other) {
      { value_type{} } noexcept;
      { value = other } noexcept;
    };

/** Requires that an argument is implicitly convertible to and can construct a
 * Vector element without throwing.
 */
template <typename value_type, typename argument_type>
concept NothrowElementConstruction =
    std::convertible_to<argument_type, value_type> &&
    requires(argument_type argument) {
      { argument_type{argument} } noexcept;
      { value_type{argument} } noexcept;
    };

/** Requires that \p collection_type exposes a \c size() member returning a
 * count of logical elements.
 */
template <typename collection_type>
concept Collection = requires(const collection_type &c) {
  { c.size() } -> std::same_as<std::size_t>;
};

} // namespace cljonic::concepts
