#ifndef CLJONIC_SET_HPP
#define CLJONIC_SET_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <span>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Set
 * \b Set is a bounded, unordered collection of unique values. It provides callable lookup with optional fallback
 * values. The way to operate on the collection is through the library's free-function API. Updates return a modified
 * copy without changing the original collection. Construction with more values than the available capacity is rejected
 * at compile time.
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"

 int main() {
   using namespace cljonic;

   using AccountId = int;
   using AccountSet = Set<AccountId, 4>;

   // A named set type makes the element and capacity contract explicit. A
   // duplicate value is a no-op. Constant-evaluated duplicate construction is
   // rejected.
   constexpr auto literal = AccountSet{1, 2, 3};
   static_assert(literal(2) == 2);
   static_assert(literal(99) == 0);
   static_assert(literal(99, -1) == -1);

   // Runtime CTAD deduces Set<int, 3> from the argument count and keeps one
   // copy when duplicate values are present.
   auto runtime = Set{10, 20, 20};
   const auto present = runtime(10);
   const auto missing = runtime(30, -1);

   // ---------------------------------------------------------------------
   // C++ interoperability: a Set exposes const logical traversal, a
   // non-owning contiguous standard view, and can be constructed from a
   // read-only std::span without mutating the source data.
   // ---------------------------------------------------------------------
   static constexpr int source_values[] = {11, 22, 11, 33};
   constexpr std::span source_span{source_values};
   constexpr auto from_span = AccountSet{source_span};
   static_assert(from_span(11) == 11);
   static_assert(from_span(22) == 22);
   static_assert(from_span(33) == 33);
   static_assert(from_span(99) == 0);

   constexpr auto from_span_ctad = Set{source_span};
   static_assert(from_span_ctad(22) == 22);
   static_assert(from_span_ctad.view().size() == 3);

   int runtime_buffer[] = {100, 200, 300};
   const auto runtime_from_span =
       Set<int, 4>{std::span<const int>{runtime_buffer, 3}};

   // Const C++ interoperability exposes the active elements as a range and
   // as a non-owning contiguous standard view. Set traversal order is not
   // semantically ordered.
   static_assert(literal.view().size() == 3);

   // Use C++ interoperability to sum the values in a set.
   int observed_sum = 0;
   for (const auto value : runtime) {
     observed_sum += value;
   }

   const auto runtime_view = runtime.view();

   return (present == 10 && missing == -1 && observed_sum == 30 &&
           runtime_view.size() == 2 && runtime_from_span(100) == 100 &&
           runtime_from_span(300) == 300)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::NothrowStableEqualityComparable T, std::size_t CapacityValue>
class Set {
  public:
    using value_type = T;
    using lookup_type = value_type;

    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "Set CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    template <typename... Args>
    constexpr Set(Args&&... args) noexcept((concepts::NothrowElementConstruction<T, Args> && ...)) {
        static_assert(sizeof...(Args) <= CapacityValue, "Set initializer count exceeds Set CapacityValue");
        static_assert((concepts::NothrowElementConstruction<T, Args> && ...),
                      "Set constructor requires all arguments to construct "
                      "T without throwing and be implicitly convertible to T");

        const auto duplicate = append_constructed(std::forward<Args>(args)...);

        if consteval {
            if (duplicate) {
                std::abort();
            }
        }
    }

    template <typename SourceElement, std::size_t Extent>
    constexpr Set(std::span<const SourceElement, Extent> source) noexcept {
        static_assert(std::convertible_to<SourceElement, value_type>,
                      "Set span constructor requires SourceElement to be implicitly convertible to T without throwing");
        if constexpr (Extent != std::dynamic_extent) {
            static_assert(Extent <= CapacityValue, "Set span source exceeds Set CapacityValue");
        }

        const auto copy_count = std::min<std::size_t>(source.size(), CapacityValue);
        for (std::size_t index = 0; index < copy_count; ++index) {
            *this = conj(value_type{source[index]});
        }
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return CapacityValue;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return logical_size_ == 0U;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const value_type* {
        return elements_.data();
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const value_type* {
        return elements_.data() + logical_size_;
    }

    [[nodiscard]] constexpr auto view() const noexcept -> std::span<const value_type> {
        return {elements_.data(), logical_size_};
    }

    [[nodiscard]] constexpr auto contains(const T& element) const noexcept -> bool {
        return find_index(element) < logical_size_;
    }

    /** Returns the stored element value when present; otherwise returns a
     * default-constructed value. */
    [[nodiscard]] constexpr auto operator()(const T& element) const noexcept -> T {
        const auto idx = find_index(element);
        return (idx < logical_size_) ? elements_[idx] : T{};
    }

    /** Returns the stored element value when present; otherwise returns the
     * supplied fallback. */
    [[nodiscard]] constexpr auto operator()(const T& element, const T& fallback) const noexcept -> T {
        const auto idx = find_index(element);
        return (idx < logical_size_) ? elements_[idx] : fallback;
    }

    /** Returns true when the element is already present or the set has room for
     * one more. */
    [[nodiscard]] constexpr auto can_conj(const T& element) const noexcept -> bool {
        return contains(element) || (logical_size_ < CapacityValue);
    }

    /** Adds an element to a copy of the set. If the element is already present or
     * capacity is full, returns an unchanged copy. */
    [[nodiscard]] constexpr auto conj(const T& element) const noexcept -> Set {
        Set result = *this;
        if (!result.contains(element) && result.logical_size_ < CapacityValue) {
            result.elements_[result.logical_size_] = element;
            ++result.logical_size_;
        }
        return result;
    }

    /** Removes an element via swap-and-remove if present; always returns a valid
     * copy. */
    [[nodiscard]] constexpr auto disj(const T& element) const noexcept -> Set {
        Set result = *this;
        const auto idx = result.find_index(element);
        if (idx < result.logical_size_) {
            if (idx + 1U < result.logical_size_) {
                result.elements_[idx] = result.elements_[result.logical_size_ - 1U];
            }
            --result.logical_size_;
        }
        return result;
    }

  private:
    template <typename Arg>
    constexpr auto append_constructed(Arg&& arg) noexcept -> bool {
        T value{std::forward<Arg>(arg)};
        if (contains(value)) {
            return true;
        }
        *this = conj(value);
        return false;
    }

    template <typename... Args>
    constexpr auto append_constructed(Args&&... args) noexcept -> bool {
        bool duplicate = false;
        ((duplicate = append_constructed(std::forward<Args>(args)) || duplicate), ...);
        return duplicate;
    }

    /** Returns `logical_size_` when the element is absent. */
    [[nodiscard]] constexpr auto find_index(const T& element) const noexcept -> std::size_t {
        for (std::size_t i = 0; i < logical_size_; ++i) {
            if (elements_[i] == element) {
                return i;
            }
        }
        return logical_size_;
    }

    std::array<value_type, CapacityValue> elements_{};
    std::size_t logical_size_{0};
};

template <typename First, typename... Rest>
Set(First, Rest...) -> Set<First, 1 + sizeof...(Rest)>;

template <typename SourceElement, std::size_t Extent>
    requires(Extent != std::dynamic_extent)
Set(std::span<const SourceElement, Extent>) -> Set<std::remove_cv_t<SourceElement>, Extent>;

template <typename SourceElement, std::size_t Extent>
    requires(Extent != std::dynamic_extent)
Set(std::span<SourceElement, Extent>) -> Set<std::remove_cv_t<SourceElement>, Extent>;

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename T, std::size_t CapacityValue>
struct collection_traits<Set<T, CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::set;
};

} // namespace cljonic::concepts_detail

#endif // CLJONIC_SET_HPP
