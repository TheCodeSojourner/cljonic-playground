#pragma once

#include <array>
#include <cstddef>

#include <concepts.hpp>

namespace cljonic
{

    enum class vector_state
    {
        unclassified,
        empty,
        populated,
        at_capacity,
    };

    namespace detail::vector
    {
        struct vector_observation
        {
            int capacity_limit;
            int logical_size;
        };

        [[nodiscard]] constexpr auto classify_vector(vector_observation observation) noexcept -> vector_state
        {
            if (observation.logical_size == 0)
            {
                return vector_state::empty;
            }

            if (observation.logical_size == observation.capacity_limit)
            {
                return vector_state::at_capacity;
            }

            return vector_state::populated;
        }
    } // namespace detail::vector

    template <concepts::VectorElement element_type, std::size_t capacity_value>
    class Vector
    {
    public:
        using value_type = element_type;

        [[nodiscard]] static constexpr auto capacity_limit() noexcept -> std::size_t
        {
            return capacity_value;
        }

        [[nodiscard]] constexpr auto logical_size() const noexcept -> std::size_t
        {
            return logical_size_;
        }

        [[nodiscard]] constexpr auto state() const noexcept -> vector_state
        {
            const detail::vector::vector_observation observation{static_cast<int>(capacity_value), static_cast<int>(logical_size_)};
            return detail::vector::classify_vector(observation);
        }

        [[nodiscard]] constexpr auto try_push_back(const value_type &value) noexcept(concepts::NothrowCopyAssignable<value_type>) -> bool
        {
            if (logical_size_ >= capacity_value)
            {
                return false;
            }

            storage_[logical_size_] = value;
            ++logical_size_;
            return true;
        }

    private:
        std::array<value_type, capacity_value> storage_{};
        std::size_t logical_size_ = 0;
    };

    template <concepts::VectorElement element_type, std::size_t capacity_value>
    [[nodiscard]] constexpr auto count(const Vector<element_type, capacity_value> &collection) noexcept -> std::size_t
    {
        return collection.logical_size();
    }

} // namespace cljonic
