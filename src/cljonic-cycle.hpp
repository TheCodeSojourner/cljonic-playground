#pragma once

#include <cstddef>
#include <ranges>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-vector.hpp>

namespace cljonic {

/** \anchor Cycle
 * \b Cycle is an unbounded producer over an owned source value. A finite source sequence is repeated from the
 * beginning after exhaustion; an unbounded source preserves its observable prefix without requiring a complete source
 * result. The approved public form is `cycle(source)` only, and traversal terminates only at the observable cap used
 * for bounded materialization.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   [[maybe_unused]] constexpr auto finite_cycle = cycle(Vector<int, 3>{1, 2, 3});
   [[maybe_unused]] constexpr auto unbounded_cycle = cycle(Range{0, 0, 0});

   // -----------------------------------------------------------------------
   // C++ interoperability: Cycle supports const traversal.
   // -----------------------------------------------------------------------
   auto runtime_cycle = cycle(Vector<int, 3>{4, 5, 6});
   auto runtime_unbounded_cycle = cycle(Range{0, 0, 0});

   // Use C++ interoperability to observe a finite source cycle at runtime.
   int runtime_sum = 0;
   int runtime_count = 0;
   for (const auto value : runtime_cycle) {
     runtime_sum += value;
     if (++runtime_count == 5) {
       break;
     }
   }

   // Use C++ interoperability to observe an unbounded source cycle at runtime.
   int runtime_unbounded_sum = 0;
   int runtime_unbounded_count = 0;
   for (const auto value : runtime_unbounded_cycle) {
     runtime_unbounded_sum += value;
     if (++runtime_unbounded_count == 4) {
       break;
     }
   }

   return runtime_count == 5 && runtime_sum == 24 &&
                  runtime_unbounded_count == 4 && runtime_unbounded_sum == 0
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::CljonicSource Source>
    requires concepts::NothrowCollectionElement<std::ranges::range_value_t<const Source>>
class Cycle {
  public:
    using source_type = Source;
    using value_type = std::ranges::range_value_t<const Source>;
    using source_iterator = std::ranges::iterator_t<const Source>;
    using source_sentinel = std::ranges::sentinel_t<const Source>;

    class const_iterator {
      public:
        using value_type = Cycle::value_type;
        using difference_type = std::ptrdiff_t;

        constexpr const_iterator() noexcept = default;

        constexpr const_iterator(const Source* source, source_iterator current, source_sentinel end,
                                 std::size_t remaining, bool restart) noexcept
            : source_(source), current_(std::move(current)), end_(std::move(end)), remaining_(remaining),
              restart_(restart) {
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> decltype(auto) {
            return *current_;
        }

        constexpr auto operator++() noexcept -> const_iterator& {
            if (remaining_ == 0U) {
                remaining_ = 0U;
                return *this;
            }
            --remaining_;
            ++current_;
            if (remaining_ != 0U && current_ == end_) {
                advance_after_source_end();
            }
            return *this;
        }

        constexpr auto operator++(int) noexcept -> const_iterator {
            auto previous = *this;
            ++(*this);
            return previous;
        }

        [[nodiscard]] friend constexpr auto operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept
            -> bool {
            return lhs.remaining_ == rhs.remaining_;
        }

      private:
        constexpr void advance_after_source_end() noexcept {
            current_ = std::ranges::begin(*source_);
            end_ = std::ranges::end(*source_);
            remaining_ = restart_ ? remaining_ : 0U;
        }

        const Source* source_{nullptr};
        source_iterator current_{};
        source_sentinel end_{};
        std::size_t remaining_{0U};
        bool restart_{false};
    };

    constexpr Cycle() noexcept = default;

    constexpr explicit Cycle(Source source) noexcept : source_(std::move(source)) {
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return count_;
    }

    [[nodiscard]] constexpr auto is_finite() const noexcept -> bool {
        return is_finite_;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        const auto source_count = source_.count();
        const auto source_begin = std::ranges::begin(source_);
        const auto source_end = std::ranges::end(source_);
        const auto source_is_finite = source_finiteness();
        if (source_count == 0U || source_begin == source_end) {
            return const_iterator{&source_, source_begin, source_end, 0U, source_is_finite};
        }
        return const_iterator{&source_, source_begin, source_end, count(), source_is_finite};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator{};
    }

    /** Producer parameter equality (REQ-FN-014B): compares only the stored
     *  owned source, count, and finite-form parameters, never the produced
     *  sequence. The owned source compares by its own stable equality, so
     *  `cycle(Vector{1, 2, 3}) == cycle(Vector{1, 2, 3})` is true while
     *  `cycle(Vector{1, 2, 3}) == cycle(Vector{1, 2, 4})` is false. O(1), no
     *  traversal, no allocation. */
    [[nodiscard]] friend constexpr auto operator==(const Cycle& lhs, const Cycle& rhs) noexcept -> bool
        requires concepts::StableEqualityComparable<Source>
    {
        return lhs.source_ == rhs.source_ && lhs.count_ == rhs.count_ && lhs.is_finite_ == rhs.is_finite_;
    }

  private:
    [[nodiscard]] constexpr auto source_finiteness() const noexcept -> bool {
        if constexpr (concepts::CljonicProducer<Source>) {
            return source_.is_finite();
        }
        return true;
    }

    Source source_{};
    std::size_t count_{CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE};
    bool is_finite_{false};
};

template <concepts::CljonicSource Source>
    requires concepts::NothrowCollectionElement<std::ranges::range_value_t<const Source>>
[[nodiscard]] constexpr auto cycle(Source source) noexcept -> Cycle<Source> {
    return Cycle<Source>{std::move(source)};
}

template <concepts::CljonicSource Source>
    requires concepts::NothrowCollectionElement<std::ranges::range_value_t<const Source>> &&
             concepts::StableEqualityComparable<Source>
[[nodiscard]] constexpr auto parameters_equal(const Cycle<Source>& lhs, const Cycle<Source>& rhs) noexcept -> bool {
    return lhs == rhs;
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename Source>
struct producer_traits<Cycle<Source>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::cycle;
};

template <concepts::CljonicSource Source>
struct contains_floating_point<Cycle<Source>> : std::bool_constant<contains_floating_point_v<Source>> {};

template <concepts::CljonicSource Source>
struct contains_callable<Cycle<Source>> : std::bool_constant<contains_callable_v<Source>> {};

} // namespace cljonic::concepts_detail
