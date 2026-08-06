#pragma once

#include <concepts>

namespace cljonic::concepts {

template <typename value_type>
concept VectorElement =
    std::default_initializable<value_type> && std::copyable<value_type>;

template <typename value_type>
concept NothrowCopyAssignable =
    requires(value_type &target, const value_type &source) {
      { target = source } noexcept -> std::same_as<value_type &>;
    };

} // namespace cljonic::concepts
