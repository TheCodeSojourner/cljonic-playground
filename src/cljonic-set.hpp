#pragma once

#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Set
 * A Set is a bounded, unordered collection that provides callable lookup with optional fallback values. Other
 * operations use the library's free-function API.
 *
 * Direct construction can import a C++ range or view: its elements are copied into the Set. A cljonic value is not
 * copied as a range/view source. With an explicit element type, a cljonic argument is accepted as one element only
 * when its type exactly matches the declared element type. Without explicit template arguments, a sole cljonic
 * argument determines the element type and yields a one-element collection. Elements may themselves be cljonic
 * collections or producers, including nested cljonic composites.
 *
 * Construction with more values than the available capacity is rejected at compile time. Elements and fallbacks follow
 * the \ref implicit-conversions "safe implicit conversion rules".
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 using AccountId = int;
 using AccountSet = Set<AccountId, 4>;

 int main() {
   // A named set type makes the element and capacity contract explicit. A
   // duplicate value is a no-op. Constant-evaluated duplicate construction is
   // rejected at compile time.
   constexpr auto literal = AccountSet{1, 2, 3};
   static_assert(literal(2) == 2);
   static_assert(literal(99) == 0);
   static_assert(literal(99, -1) == -1);

   // Runtime CTAD deduces Set<int, 3> from the argument count and keeps one
   // copy when duplicate values are present.
   auto runtime = Set{10, 20, 20};
   const auto present = runtime(10);
   const auto missing = runtime(30, -1);

   // A standard view pipeline can use an existing Set as its source and
   // Construct another Set from the transformed values.
   const auto doubled_view =
       runtime |
       std::views::transform([](int value) noexcept { return value * 2; });
   const auto from_pipeline = AccountSet{doubled_view};

   // -------------------------------------------------------------------------
   // C++ interoperability: a Set supports const traversal and exposes a
   // non-owning std::span view. Range/view sources are copied into owned
   // storage, retaining only the bounded prefix that fits the capacity.
   // -------------------------------------------------------------------------
   static constexpr int source_values[] = {11, 22, 11, 33};
   constexpr auto source_span = std::span{source_values};
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
   auto observed_sum = 0;
   for (const auto value : runtime) {
     observed_sum += value;
   }

   const auto runtime_view = runtime.view();

   return (present == 10 && missing == -1 && observed_sum == 30 &&
           from_pipeline(20) == 20 && runtime_view.size() == 2 &&
           runtime_from_span(100) == 100 && runtime_from_span(300) == 300)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::NothrowStableEqualityComparable T, std::size_t CapacityValue>
    requires(!concepts_detail::is_std_variant_v<T>)
class Set {
  public:
    using value_type = T;
    using lookup_type = value_type;

    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "cljonic::Set: CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    template <typename... Args>
    explicit(sizeof...(Args) ==
             1) constexpr Set(Args&&... args) noexcept((concepts::NothrowElementConstruction<T, Args> && ...)) {
        static_assert(sizeof...(Args) <= CapacityValue, "cljonic::Set: initializer count exceeds Set CapacityValue");
        static_assert((concepts::NothrowElementConstruction<T, Args> && ...),
                      "cljonic::Set: constructor requires all arguments to construct "
                      "T without throwing and convert safely without losing values or changing their meaning");

        const auto duplicate = append_constructed(std::forward<Args>(args)...);

        if consteval {
            if (duplicate) {
                rejected_duplicate_value_at_compile_time();
            }
        }
    }

    // A cljonic collection or producer is not a SourceConstruction source
    // (REQ-FN-027A); materialize it via into, preflighted by fits_into.
    template <std::ranges::input_range SourceRange>
        requires(!concepts_detail::is_cljonic_collection_v<SourceRange> &&
                 !concepts_detail::is_cljonic_producer_v<SourceRange> &&
                 !std::same_as<std::remove_cvref_t<SourceRange>, value_type>)
    explicit constexpr Set(SourceRange&& source) noexcept(
        (concepts::NothrowRangeConstruction<value_type, SourceRange>)) {
        static_assert(concepts::NothrowRangeConstruction<value_type, SourceRange>,
                      "cljonic::Set: range/view constructor requires each source element to construct "
                      "T safely and requires non-throwing traversal");
        static_assert(concepts_detail::static_extent_fits_v<SourceRange, CapacityValue>,
                      "cljonic::Set: static-extent range source exceeds Set CapacityValue");

        std::size_t copy_count = 0;
        for (auto&& item : std::forward<SourceRange>(source)) {
            if (copy_count++ >= CapacityValue) {
                break;
            }
            *this = conj(value_type(std::forward<decltype(item)>(item)));
        }
    }

    // Diagnostic fallback (REQ-DIAG-010): a cljonic collection or producer is
    // not a SourceConstruction source. This overload exists only to explain the
    // rejection -- it produces no value and is never a supported call target.
    template <typename SourceValue>
        requires((concepts_detail::is_cljonic_collection_v<SourceValue> ||
                  concepts_detail::is_cljonic_producer_v<SourceValue>) &&
                 !std::same_as<std::remove_cvref_t<SourceValue>, value_type>)
    explicit constexpr Set([[maybe_unused]] SourceValue&& source) noexcept {
        static_assert(concepts_detail::dependent_false<SourceValue>,
                      "cljonic::Set: you cannot construct a Set from a cljonic collection or producer. A Set is built "
                      "from its individual elements, or from a non-cljonic C++ range or view. To keep it as a single "
                      "element, write Set{...}.");
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return CapacityValue;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    [[nodiscard]] constexpr auto operator==(const Set& other) const noexcept -> bool
        requires concepts::StableEqualityComparable<T>
    {
        if (logical_size_ != other.logical_size_) {
            return false;
        }
        for (std::size_t i = 0; i < logical_size_; ++i) {
            if (other.find_index(elements_[i]) >= other.logical_size_) {
                return false;
            }
        }
        return true;
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

    /** Returns the stored element value when present; otherwise returns the
     * supplied fallback. */
    [[nodiscard]] constexpr auto operator()(const T& element, const T& fallback = T{}) const noexcept -> T {
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

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<T, Value>)
    auto contains(const Value& value) const -> bool = delete;

    template <typename Value, typename Fallback = T>
        requires(!concepts::SafeImplicitConversion<T, Value> || !concepts::SafeImplicitConversion<T, Fallback>)
    auto operator()(const Value& value, const Fallback& fallback = Fallback{}) const -> T = delete;

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<T, Value>)
    auto can_conj(const Value& value) const -> bool = delete;

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<T, Value>)
    auto conj(const Value& value) const -> Set = delete;

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<T, Value>)
    auto disj(const Value& value) const -> Set = delete;

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

    // Compile-time-only rejection trap (REQ-COLL-019B): a Set pack construction
    // with a duplicate value is ill-formed when evaluated as a constant
    // expression, mirroring Clojure's reader error for a `#{...}` literal with a
    // duplicate key. Declared but never defined, and reached only inside
    // `if consteval`, so ordinary runtime construction deduplicates
    // (REQ-COLL-019) without a runtime diagnostic.
    static void rejected_duplicate_value_at_compile_time() noexcept;

    std::array<value_type, CapacityValue> elements_{};
    std::size_t logical_size_{0};
};

template <typename First, typename... Rest>
Set(First, Rest...) -> Set<First, 1 + sizeof...(Rest)>;

// EnclosureConstruction (REQ-FN-027A): a sole Set argument wraps as one element.
template <typename ElementType, std::size_t CapacityValue>
Set(Set<ElementType, CapacityValue>) -> Set<Set<ElementType, CapacityValue>, 1>;

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

template <typename T, std::size_t CapacityValue>
struct contains_floating_point<Set<T, CapacityValue>> : std::bool_constant<contains_floating_point_v<T>> {};

template <typename T, std::size_t CapacityValue>
struct contains_callable<Set<T, CapacityValue>> : std::bool_constant<contains_callable_v<T>> {};

template <typename T, std::size_t CapacityValue>
struct contains_standard_range<Set<T, CapacityValue>> : std::bool_constant<contains_standard_range_v<T>> {};

} // namespace cljonic::concepts_detail
