#pragma once

#include <concepts>

namespace cljonic::concepts {

/** Requires that \p value_type is default-initializable and copyable — the
 * minimum contract for \ref Vector element storage and \b SentinelBasedAccess
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

} // namespace cljonic::concepts
