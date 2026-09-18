#ifndef CLJONIC_CONCEPTS_HPP
#define CLJONIC_CONCEPTS_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>

namespace cljonic {

// Private compile-time metadata used by the public concepts below. Collection
// implementations specialize these traits to opt into the closed nominal
// collection domain; extent helpers validate bounded source types without
// inspecting runtime data.
namespace concepts_detail {

// A small closed-world tag lets concepts distinguish collection families
// without exposing implementation-specific type traits as public API.
enum class collection_kind { none, vector, map, set, queue, string };

// Convert an integral API index only when its value is representable by the
// library's normalized size_t index domain. Collection-specific bounds remain
// at each collection's call site because lookup and association have different
// valid ranges.
template <std::integral IndexType>
[[nodiscard]] constexpr auto try_normalize_index(IndexType index) noexcept -> std::optional<std::size_t> {
    if (!std::in_range<std::size_t>(index)) {
        return std::nullopt;
    }
    return static_cast<std::size_t>(index);
}

// The unspecialized form rejects types by default. Each supported collection
// specializes this trait with its nominal identity and collection kind.
template <typename T>
struct collection_traits {
    static constexpr bool is_cljonic_collection = false;
    static constexpr collection_kind kind = collection_kind::none;
};

// Remove cv/ref qualifiers so concepts behave consistently for values,
// references, and const references.
template <typename T>
inline constexpr bool is_cljonic_collection_v = collection_traits<std::remove_cvref_t<T>>::is_cljonic_collection;

template <typename T>
inline constexpr collection_kind collection_kind_of_v = collection_traits<std::remove_cvref_t<T>>::kind;

// Extract a source's compile-time element count when one is knowable. A
// dynamic extent remains acceptable because it must be checked at runtime.
template <typename T>
struct static_extent : std::integral_constant<std::size_t, std::dynamic_extent> {};

template <typename ElementType, std::size_t Extent>
struct static_extent<std::span<ElementType, Extent>> : std::integral_constant<std::size_t, Extent> {};

template <typename ElementType, std::size_t Extent>
struct static_extent<std::array<ElementType, Extent>> : std::integral_constant<std::size_t, Extent> {};

template <typename ElementType, std::size_t Extent>
struct static_extent<ElementType[Extent]> : std::integral_constant<std::size_t, Extent> {};

template <typename T>
inline constexpr std::size_t static_extent_v = static_extent<std::remove_cvref_t<T>>::value;

// Static sources must fit their destination capacity before construction;
// dynamic sources defer that decision to the bounded copy loop.
template <typename T, std::size_t CapacityValue>
inline constexpr bool static_extent_fits_v =
    static_extent_v<T> == std::dynamic_extent || static_extent_v<T> <= CapacityValue;

// A closed-world tag distinguishing producer families, parallel to collection_kind
// but for the separate producer nominal domain (cljonic_source ≡ collection ∨ producer).
enum class producer_kind { none, range, repeat };

// The unspecialized form rejects types by default. Each supported producer
// specializes this trait with its nominal identity and producer kind.
template <typename T>
struct producer_traits {
    static constexpr bool is_cljonic_producer = false;
    static constexpr producer_kind kind = producer_kind::none;
};

template <typename T>
inline constexpr bool is_cljonic_producer_v = producer_traits<std::remove_cvref_t<T>>::is_cljonic_producer;

template <typename T>
inline constexpr producer_kind producer_kind_of_v = producer_traits<std::remove_cvref_t<T>>::kind;

} // namespace concepts_detail

namespace concepts {

// ============================================================================
// Storage & Element Capability Concepts
// ============================================================================

/** Requires that \p T is default-initializable and copyable. */
template <typename T>
concept CopyableElement = std::default_initializable<T> && std::copyable<T>;

/** Requires that all collection storage lifetime and copy operations do not throw. */
template <typename T>
concept NothrowCollectionElement = CopyableElement<T> && std::destructible<T> && requires(T value, const T& other) {
    { T{} } noexcept;
    { T{other} } noexcept;
    { value = other } noexcept;
};

/** Requires that an argument is convertible to and can construct an element
 *  without throwing. */
template <typename T, typename Arg>
concept NothrowElementConstruction = std::convertible_to<Arg, T> && requires(Arg&& argument) {
    { T{std::forward<Arg>(argument)} } noexcept;
};

// ============================================================================
// Value Capability Concepts
// ============================================================================

/** Requires stable value equality comparison, explicitly rejecting
 *  floating-point types to prevent NaN/precision instabilities in map keys
 *  and set elements. */
template <typename T>
concept StableEqualityComparable = std::equality_comparable<T> && !std::floating_point<std::remove_cvref_t<T>>;

/** Requires a strict total ordering layered on stable equality. */
template <typename T>
concept TotallyOrdered = StableEqualityComparable<T> && std::totally_ordered<T>;

/** Requires stable equality combined with non-throwing collection storage,
 *  the admission contract shared by map keys and set elements. */
template <typename T>
concept NothrowStableEqualityComparable = StableEqualityComparable<T> && NothrowCollectionElement<T>;

// ============================================================================
// Level 1: CollectionConcept (Nominal Collection Admission)
// ============================================================================

/** Gates types admitted to the closed nominal cljonic collection domain
 *  through cljonic-owned trait specialization. */
template <typename T>
concept CljonicCollection = concepts_detail::is_cljonic_collection_v<T>;

/** Gates types admitted to the separate producer nominal domain through
 *  cljonic-owned trait specialization, distinct from CljonicCollection. */
template <typename T>
concept CljonicProducer = concepts_detail::is_cljonic_producer_v<T>;

/** Nominal identity gate for Range producer types. */
template <typename T>
concept CljonicRange =
    CljonicProducer<T> && (concepts_detail::producer_kind_of_v<T> == concepts_detail::producer_kind::range);

/** Nominal identity gate for Repeat producer types. */
template <typename T>
concept CljonicRepeat =
    CljonicProducer<T> && (concepts_detail::producer_kind_of_v<T> == concepts_detail::producer_kind::repeat);

/** Admits either a stored collection or a producer to the combined source
 *  domain used by materialization operations (`into`, `fits_into`). */
template <typename T>
concept CljonicSource = CljonicCollection<T> || CljonicProducer<T>;

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
 * is_empty() and count() sequence observation, with count() returning std::size_t. */
template <typename C>
concept SequenceableCollection = CljonicCollection<C> && requires(const C& c) {
    { c.is_empty() } noexcept -> std::same_as<bool>;
    { c.count() } noexcept -> std::same_as<std::size_t>;
};

/** Requires that an admitted collection provides callable indexed lookup
 *  c(index) and the contains(index) index-in-range membership test (Clojure
 *  contains? over vector/string indices). */
template <typename C>
concept IndexedCollection = CljonicCollection<C> && requires(const C& c, std::size_t i) {
    { c(i) } noexcept;
    { c.contains(i) } noexcept -> std::same_as<bool>;
};

/** Requires an admitted collection to expose a named lookup
 * domain, callable lookup, and matching membership predicate. */
template <typename C>
concept LookupCollection = CljonicCollection<C> && requires(const C& c, const C::lookup_type& key) {
    { c(key) } noexcept;
    { c.contains(key) } noexcept -> std::same_as<bool>;
};

/** Requires that an admitted collection provides immutable association and
 *  its key-domain/capacity preflight operation. */
template <typename C>
concept AssociativeCollection =
    CljonicCollection<C> && requires(const C& c, const C::key_type& key, const C::association_value_type& value) {
        typename C::key_type;
        typename C::association_value_type;
        { c.can_assoc(key) } noexcept -> std::same_as<bool>;
        { c.assoc(key, value) } noexcept -> std::same_as<C>;
    };

/** Requires that an admitted producer provides non-throwing count() effective-size
 *  observation, returning std::size_t. count() for a producer is a conservative
 *  materialization maximum (saturated at the synthesis cap), not necessarily the
 *  true unsaturated span. */
template <typename C>
concept SequenceableProducer = CljonicProducer<C> && requires(const C& c) {
    { c.count() } noexcept -> std::same_as<std::size_t>;
};

/** Requires that an admitted producer provides the contains(i) index-in-range predicate
 *  over the available bounded prefix, in O(1) without traversal (e.g. Range). Unlike
 *  IndexedCollection, this does not require callable value access: a
 *  Range is Indexed but not IFn (invocable), unlike Vector/Map/Set. Positional value
 *  retrieval is deferred future work. Cycle and Iterate never qualify; Repeat and
 *  Repeatedly never qualify either (they are not efficiently indexed in Clojure). */
template <typename C>
concept IndexedProducer = CljonicProducer<C> && requires(const C& c, std::size_t i) {
    { c.contains(i) } noexcept -> std::same_as<bool>;
};

} // namespace concepts

} // namespace cljonic

#endif // CLJONIC_CONCEPTS_HPP
