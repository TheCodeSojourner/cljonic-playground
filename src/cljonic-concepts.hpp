#ifndef CLJONIC_CONCEPTS_HPP
#define CLJONIC_CONCEPTS_HPP

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace cljonic {

namespace concepts_detail {

enum class collection_kind { none, vector, map, set, queue, string };

template <typename T>
struct collection_traits {
    static constexpr bool is_cljonic_collection = false;
    static constexpr collection_kind kind = collection_kind::none;
};

template <typename T>
inline constexpr bool is_cljonic_collection_v = collection_traits<std::remove_cvref_t<T>>::is_cljonic_collection;

template <typename T>
inline constexpr collection_kind collection_kind_of_v = collection_traits<std::remove_cvref_t<T>>::kind;

} // namespace concepts_detail

namespace concepts {

// ============================================================================
// Storage & Element Capability Concepts
// ============================================================================

/** Requires that \p T is default-initializable and copyable. */
template <typename T>
concept CopyableElement = std::default_initializable<T> && std::copyable<T>;

/** Requires that element storage operations (default construct, copy construct,
 *  copy assign) do not throw exceptions. */
template <typename T>
concept NothrowCopyableElement = CopyableElement<T> && requires(T value, const T& other) {
    { T{} } noexcept;
    { T{other} } noexcept;
    { value = other } noexcept;
};

/** Requires that an argument is convertible to and can construct an element
 *  without throwing. */
template <typename T, typename Arg>
concept NothrowElementConstruction = std::convertible_to<Arg, T> && requires(Arg argument) {
    { Arg{argument} } noexcept;
    { T{argument} } noexcept;
};

// Backward-compatible aliases for existing container templates during Phase C.
template <typename T>
concept VectorElement = CopyableElement<T>;

template <typename T>
concept NothrowVectorElement = NothrowCopyableElement<T>;

// ============================================================================
// Value Capability Concepts
// ============================================================================

/** Requires stable value equality comparison, explicitly rejecting
 *  floating-point types to prevent NaN/precision instabilities in map keys
 *  and set elements. */
template <typename T>
concept StableEqualityComparable = requires(const T& a, const T& b) {
    { a == b } -> std::same_as<bool>;
} && !std::is_floating_point_v<std::remove_cvref_t<T>>;

/** Requires a strict total ordering layered on stable equality. */
template <typename T>
concept TotallyOrdered = StableEqualityComparable<T> && requires(const T& a, const T& b) {
    { a < b } -> std::same_as<bool>;
};

// ============================================================================
// Level 1: CollectionConcept (Nominal Collection Admission)
// ============================================================================

/** Gates types admitted to the closed nominal cljonic collection domain
 *  through cljonic-owned trait specialization. */
template <typename T>
concept CljonicCollection = concepts_detail::is_cljonic_collection_v<T>;

/** Nominal identity gate for Vector collection types. */
template <typename T>
concept CljonicVector =
    CljonicCollection<T> && (concepts_detail::collection_kind_of_v<T> == concepts_detail::collection_kind::vector);

/** Nominal identity gate for Map collection types. */
template <typename T>
concept CljonicMap =
    CljonicCollection<T> && (concepts_detail::collection_kind_of_v<T> == concepts_detail::collection_kind::map);

/** Nominal identity gate for Set collection types. */
template <typename T>
concept CljonicSet =
    CljonicCollection<T> && (concepts_detail::collection_kind_of_v<T> == concepts_detail::collection_kind::set);

/** Nominal identity gate for Queue collection types. */
template <typename T>
concept CljonicQueue =
    CljonicCollection<T> && (concepts_detail::collection_kind_of_v<T> == concepts_detail::collection_kind::queue);

/** Nominal identity gate for String collection types. */
template <typename T>
concept CljonicString =
    CljonicCollection<T> && (concepts_detail::collection_kind_of_v<T> == concepts_detail::collection_kind::string);

// ============================================================================
// Level 2: CapabilityConcept (Structural Collection Capabilities)
// ============================================================================

/** Requires that an admitted nominal collection provides non-throwing
 * is_empty() and count() sequence observation. */
template <typename C>
concept SequenceableCollection = CljonicCollection<C> && requires(const C& c) {
    { c.is_empty() } noexcept -> std::same_as<bool>;
    { c.count() } noexcept -> std::integral;
};

/** Requires that a sequenceable collection provides callable indexed lookup
 *  c(index) and the valid_index(index) preflight predicate. */
template <typename C>
concept IndexedCollection = SequenceableCollection<C> && requires(const C& c, std::size_t i) {
    { c(i) } noexcept;
    { c.valid_index(i) } noexcept -> std::same_as<bool>;
};

/** Requires that a sequenceable collection provides callable key lookup
 *  c(key) and the contains(key) membership test. */
template <typename C>
concept AssociativeCollection = SequenceableCollection<C> && requires(const C& c, const typename C::key_type& k) {
    { c(k) } noexcept;
    { c.contains(k) } noexcept -> std::same_as<bool>;
};

} // namespace concepts

} // namespace cljonic

#endif // CLJONIC_CONCEPTS_HPP
