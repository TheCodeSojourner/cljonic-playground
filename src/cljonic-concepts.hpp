#pragma once

#include <concepts>
#include <utility>

namespace cljonic::concepts {

/** Requires that \p value_type is default-initializable and copyable - the
 * minimum contract for Vector element storage and SentinelBasedAccess
 * default-element return.
 */
template <typename value_type>
concept VectorElement =
    std::default_initializable<value_type> && std::copyable<value_type>;

/** Detects whether copy-assignment of \p value_type is \c noexcept, used to
 * propagate \c noexcept on \c try_push_back.
 */
template <typename value_type>
concept NothrowCopyAssignable =
    requires(value_type &target, const value_type &source) {
      { target = source } noexcept -> std::same_as<value_type &>;
    };

/** Requires that \\p argument_type is convertible to \\p value_type.
 */
template <typename value_type, typename argument_type>
concept ConvertibleToElement = std::convertible_to<argument_type, value_type>;

/** Requires that \p value_type can be constructed from \p argument_type
 * without throwing.
 */
template <typename value_type, typename argument_type>
concept NothrowConstructible =
    std::constructible_from<value_type, argument_type> && requires {
      { value_type(std::declval<argument_type>()) } noexcept;
    };

/** Requires that \p collection_type exposes a \c size() member returning a
 * count of logical elements.
 */
template <typename collection_type>
concept Collection = requires(const collection_type &c) {
  { c.size() } -> std::convertible_to<std::size_t>;
};

} // namespace cljonic::concepts
