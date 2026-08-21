#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>

#include <cljonic-concepts.hpp>

namespace cljonic
{

    /** \anchor Range
     * \b Range is a bounded integer producer that represents a sequence from
     * \p start up to but excluding \p end, advancing by \p step. The supported
     * subset is intentionally limited to integral types with explicit defaulting and
     * bounded materialization.
     */
    template <std::integral value_type>
    class Range
    {
    public:
        using element_type = value_type;

        static_assert(std::integral<value_type>,
                      "Range requires an integral element type");

        constexpr Range() noexcept : start_{0}, end_{0}, step_{1}, size_{0} {}

        constexpr explicit Range(value_type end) noexcept
            : Range(value_type{0}, end, value_type{1}) {}

        constexpr Range(value_type start, value_type end) noexcept
            : Range(start, end, value_type{1}) {}

        constexpr Range(value_type start, value_type end, value_type step) noexcept
            : start_{start}, end_{end}, step_{step}, size_{compute_size(start, end, step)}
        {
            if (step == value_type{0})
            {
                start_ = start;
                end_ = end;
                step_ = step;
                size_ = 0U;
            }
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return size_;
        }

        [[nodiscard]] constexpr auto empty() const noexcept -> bool
        {
            return size_ == 0U;
        }

        [[nodiscard]] constexpr auto begin() const noexcept -> value_type
        {
            return empty() ? value_type{} : start_;
        }

        [[nodiscard]] constexpr auto end() const noexcept -> value_type
        {
            return end_;
        }

        [[nodiscard]] constexpr auto step() const noexcept -> value_type
        {
            return step_;
        }

    private:
        static constexpr auto compute_size(value_type start, value_type end,
                                           value_type step) noexcept -> std::size_t
        {
            if (step == value_type{0})
            {
                return 0U;
            }

            if (step > value_type{0})
            {
                if (start >= end)
                {
                    return 0U;
                }
                const auto delta = static_cast<std::make_unsigned_t<value_type>>(end - start);
                const auto positive_step =
                    static_cast<std::make_unsigned_t<value_type>>(step);
                return delta == 0U ? 0U : ((delta + positive_step - 1U) / positive_step);
            }

            if (start <= end)
            {
                return 0U;
            }

            const auto magnitude = static_cast<std::make_unsigned_t<value_type>>(-step);
            const auto delta = static_cast<std::make_unsigned_t<value_type>>(start - end);
            return delta == 0U ? 0U : ((delta + magnitude - 1U) / magnitude);
        }

        value_type start_;
        value_type end_;
        value_type step_;
        std::size_t size_;
    };

    template <std::integral integer_type>
    Range(integer_type) -> Range<integer_type>;

    template <std::integral integer_type>
    Range(integer_type, integer_type) -> Range<integer_type>;

    template <std::integral integer_type>
    Range(integer_type, integer_type, integer_type) -> Range<integer_type>;

} // namespace cljonic
