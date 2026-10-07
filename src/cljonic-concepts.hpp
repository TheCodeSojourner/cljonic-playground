#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <optional>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <variant>

namespace cljonic {

// Private compile-time metadata used by the public concepts below. Collection
// implementations specialize these traits to opt into the closed nominal
// collection domain; extent helpers validate bounded source types without
// inspecting runtime data.
namespace concepts_detail {

// A small closed-world tag lets concepts distinguish collection families
// without exposing implementation-specific type traits as public API.
enum class collection_kind { none, vector, map, set, queue, string };

// A template-dependent false value for instantiation-dependent static_assert
// diagnostics (REQ-DIAG-009): the assertion is ill-formed only where the
// enclosing template is instantiated, so uninstantiated diagnostic overloads
// never fire.
template <typename...>
inline constexpr bool dependent_false = false;

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
// but for the separate producer nominal domain (CljonicSource ≡ collection ∨ producer).
enum class producer_kind { none, range, repeat, cycle, iterate, repeatedly };

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

// Nominal admission for the cljonic composite value type cljonic::Variant
// (REQ-CAP-011). The Variant header specializes this trait; the unspecialized
// form rejects types by default, mirroring collection_traits and
// producer_traits. std::variant is NOT admitted: it is not a cljonic value type.
template <typename T>
struct cljonic_variant_traits {
    static constexpr bool is_cljonic_variant = false;
};

template <typename T>
inline constexpr bool is_cljonic_variant_v = cljonic_variant_traits<std::remove_cvref_t<T>>::is_cljonic_variant;

// The standard-library variant is NOT a cljonic value type (REQ-CAP-011): it is
// rejected as an equality operand and as a map key or set element. It may be
// used only as an internal implementation detail of cljonic::Variant.
template <typename T>
struct is_std_variant : std::false_type {};

template <typename... Alternatives>
struct is_std_variant<std::variant<Alternatives...>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_variant_v = is_std_variant<std::remove_cvref_t<T>>::value;

// Recursive component analysis for composite values in the closed cljonic
// value domain (REQ-CAP-010). `contains_floating_point_v<T>` reports whether a
// floating-point type occurs as T itself or inside any stored component of a
// supported composite (cljonic::Variant alternatives, cljonic collections, and
// MapEntry). `contains_callable_v<T>` reports whether a callable type occurs as
// a component of a composite (including function pointers); callables never
// admit stable value equality.
template <typename T>
struct contains_floating_point : std::bool_constant<std::floating_point<std::remove_cvref_t<T>>> {};

template <typename T>
inline constexpr bool contains_floating_point_v = contains_floating_point<std::remove_cvref_t<T>>::value;

template <typename T>
struct contains_callable : std::false_type {};

template <typename T>
inline constexpr bool contains_callable_v = contains_callable<std::remove_cvref_t<T>>::value;

// A function pointer is callable even though it happens to define operator==;
// the callable-component rejection rule treats it as a callable for composite
// admission (REQ-CAP-010) so address-as-key semantics are never exposed.
template <typename Return, typename... Args>
struct contains_callable<Return (*)(Args...)> : std::true_type {};

template <typename Return, typename... Args>
struct contains_callable<Return (*)(Args...) noexcept> : std::true_type {};

// Non-throwing equality of a type's own comparison (REQ-FN-002G): the type's own
// operator== must be declared noexcept. This is the recursive guarantee behind
// operations declared noexcept; a cljonic::Variant declares its own noexcept
// ==, so detection succeeds through that operator.
template <typename T>
inline constexpr bool nothrow_equality_v = requires(const T& left, const T& right) {
    { left == right } noexcept -> std::convertible_to<bool>;
};

// A standard-library range or container is outside the equality domain: `equal`
// is not part of the C++ interoperability surface, so a standard range is
// rejected wherever it occurs as an operand or a stored component, at any
// depth. Cljonic collections and producers are themselves ranges, so the base
// case excludes them and each one recurses into its component types instead.
template <typename T>
struct contains_standard_range
    : std::bool_constant<std::ranges::range<T> && !is_cljonic_collection_v<T> && !is_cljonic_producer_v<T>> {};

template <typename T>
inline constexpr bool contains_standard_range_v = contains_standard_range<std::remove_cvref_t<T>>::value;

} // namespace concepts_detail

namespace concepts {

// ============================================================================
// Storage & Element Capability Concepts
// ============================================================================

/** Requires that \p T has a default value and is copyable. */
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
 *  and set elements. For composite values this is recursive: every stored
 *  component must admit stable equality, callable components are always
 *  rejected, and the composite's own equality must be valid. */
template <typename T>
concept StableEqualityComparable = std::equality_comparable<T> && !concepts_detail::contains_floating_point_v<T> &&
                                   !concepts_detail::contains_callable_v<T>;

/** Requires a strict total ordering layered on stable equality. */
template <typename T>
concept TotallyOrdered = StableEqualityComparable<T> && std::totally_ordered<T>;

/** Requires stable equality whose comparison cannot throw, so operations
 *  declared \c noexcept (such as \c equal and \c not_equal) cannot terminate
 *  through a throwing \c operator==. std::variant relational operators are not
 *  declared noexcept by the standard, so composites recurse over their
 *  alternatives. */
template <typename T>
concept NothrowEqualityComparable = StableEqualityComparable<T> && concepts_detail::nothrow_equality_v<T>;

/** Requires stable equality combined with non-throwing collection storage,
 *  the admission contract shared by map keys and set elements. */
template <typename T>
concept NothrowStableEqualityComparable = StableEqualityComparable<T> && NothrowCollectionElement<T>;

/** Requires a `cljonic::Variant` alternative admissible for storage:
 *  non-throwing collection storage. Floating-point and callable alternatives
 *  are permitted for storage because storage does not require equality. */
template <typename T>
concept NothrowVariantAlternative = NothrowCollectionElement<T>;

/** Requires a `cljonic::Variant` alternative admissible for equality:
 *  non-throwing storage AND a non-throwing comparison, so a `cljonic::Variant`
 *  declared `noexcept` cannot terminate through a throwing `operator==`. */
template <typename T>
concept ComparableVariantAlternative = NothrowStableEqualityComparable<T> && NothrowEqualityComparable<T>;

// ============================================================================
// Level 1: CollectionConcept (Nominal Collection Admission)
// ============================================================================

/** Requires input-range traversal from a const source expression. */
template <typename T>
concept ConstInputRange = std::ranges::input_range<const T>;

/** Requires non-throwing input-range traversal from a const source expression. */
template <typename T>
concept NothrowConstInputRange =
    ConstInputRange<T> &&
    requires(const T& source, std::ranges::iterator_t<const T> iterator, std::ranges::sentinel_t<const T> sentinel) {
        { std::ranges::begin(source) } noexcept;
        { std::ranges::end(source) } noexcept;
        { *iterator } noexcept;
        { ++iterator } noexcept;
        { iterator++ } noexcept;
        { iterator == sentinel } noexcept -> std::same_as<bool>;
    };

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

/** Nominal identity gate for Cycle producer types. */
template <typename T>
concept CljonicCycle =
    CljonicProducer<T> && (concepts_detail::producer_kind_of_v<T> == concepts_detail::producer_kind::cycle);

/** Nominal identity gate for Iterate producer types. */
template <typename T>
concept CljonicIterate =
    CljonicProducer<T> && (concepts_detail::producer_kind_of_v<T> == concepts_detail::producer_kind::iterate);

/** Nominal identity gate for Repeatedly producer types. */
template <typename T>
concept CljonicRepeatedly =
    CljonicProducer<T> && (concepts_detail::producer_kind_of_v<T> == concepts_detail::producer_kind::repeatedly);

/** Requires a nothrow-storable element and a copyable, non-throwing const step transition from T to T. */
template <typename T, typename Step>
concept IterateStep = NothrowCollectionElement<T> && std::copy_constructible<Step> &&
                      std::is_nothrow_copy_constructible_v<Step> && requires(const Step& step, T value) {
                          { std::invoke(step, value) } noexcept -> std::same_as<T>;
                      };

/** Requires a nothrow-storable element and a copyable, non-throwing const zero-argument callback producing T. */
template <typename T, typename Step>
concept RepeatedlyStep = NothrowCollectionElement<T> && std::copy_constructible<Step> &&
                         std::is_nothrow_copy_constructible_v<Step> && requires(const Step& step) {
                             { std::invoke(step) } noexcept -> std::same_as<T>;
                         };

/** Admits either a stored collection or a producer to the combined source
 *  domain used by `into` materialization and its `fits_into` preflight. */
template <typename T>
concept CljonicSource = (CljonicCollection<T> || CljonicProducer<T>) && NothrowConstInputRange<T>;

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
 * domain, callable lookup, and matching membership predicate. The member-type
 * reference lives in the requires-expression body, never its parameter list, so
 * the concept stays SFINAE-friendly for a type that lacks `lookup_type`
 * (REQ-DIAG-001): naming it in the parameter list makes evaluation hard-error
 * instead of yielding false, which would break boundary-constrained free
 * functions such as `get`. */
template <typename C>
concept LookupCollection = CljonicCollection<C> && requires(const C& c) {
    typename C::lookup_type;
    { c(std::declval<const typename C::lookup_type&>()) } noexcept;
    { c.contains(std::declval<const typename C::lookup_type&>()) } noexcept -> std::same_as<bool>;
};

/** Requires that an admitted collection provides immutable association and
 *  its key-domain/capacity preflight operation. The member-type references live
 *  in the requires-expression body, never its parameter list, so the concept
 *  stays SFINAE-friendly for a type that lacks `key_type`/
 *  `association_value_type` (REQ-DIAG-001): naming them in the parameter list
 *  makes evaluation hard-error instead of yielding false, which would break
 *  boundary-constrained free functions such as `assoc`. */
template <typename C>
concept AssociativeCollection = CljonicCollection<C> && requires(const C& c) {
    typename C::key_type;
    typename C::association_value_type;
    { c.can_assoc(std::declval<const typename C::key_type&>()) } noexcept -> std::same_as<bool>;
    {
        c.assoc(std::declval<const typename C::key_type&>(), std::declval<const typename C::association_value_type&>())
    } noexcept -> std::same_as<C>;
};

/** Requires that an admitted collection provides immutable element insertion
 *  plus its matching capacity/element preflight operation. A `Conjable`
 *  collection is one of Vector, Set, Map, Queue, or String. The value-type
 *  reference lives in the requires-expression body,
 *  never its parameter list, so the concept stays SFINAE-friendly
 *  (REQ-DIAG-001). */
template <typename C>
concept ConjableCollection = CljonicCollection<C> && requires(const C& c) {
    typename C::value_type;
    { c.conj(std::declval<const typename C::value_type&>()) } noexcept -> std::same_as<C>;
    { c.can_conj(std::declval<const typename C::value_type&>()) } noexcept -> std::same_as<bool>;
};

/** Requires that an admitted producer provides non-throwing count() effective-size
 *  observation, returning std::size_t. count() for a producer is a conservative
 *  materialization maximum (saturated at the synthesis cap), not necessarily the
 *  true unsaturated span. */
template <typename C>
concept SequenceableProducer = CljonicProducer<C> && requires(const C& c) {
    { c.count() } noexcept -> std::same_as<std::size_t>;
};

/** Requires that an admitted producer provides non-throwing positional value access
 *  and a matching contains(i) predicate over the same bounded index domain. A
 *  contains(i) predicate alone reports only bounded-observation availability and
 *  does not make a producer Indexed. No currently supported producer provides
 *  positional value retrieval. */
template <typename C>
concept IndexedProducer = CljonicProducer<C> && requires(const C& c, std::size_t i) {
    { c(i) } noexcept;
    { c.contains(i) } noexcept -> std::same_as<bool>;
};

} // namespace concepts

namespace concepts_detail {

// Guarded admission helpers for the REQ-DIAG-009 producer-factory diagnostics.
// Each derives the element type internally, so an argument outside the producer
// domain yields `false` rather than an ill-formed substitution; the negation is
// therefore usable as a diagnostic-fallback constraint.

/** Placeholder return type for producer-factory diagnostic fallbacks
 *  (REQ-DIAG-009): a rejected call never produces a value; the fallback exists
 *  only to emit the targeted static_assert. A concrete type keeps `auto`
 *  deduction well-formed so the static_assert is the sole diagnostic. */
struct RejectedProducerFactory {};

/** Guarded `repeatedly` admission: a copyable, nothrow-copy-constructible
 *  callable whose non-throwing zero-argument invocation produces a
 *  nothrow-storable element. */
template <typename Step>
concept ValidRepeatedlyStep =
    std::copy_constructible<std::decay_t<Step>> && std::is_nothrow_copy_constructible_v<std::decay_t<Step>> &&
    requires(const std::decay_t<Step>& step) {
        { std::invoke(step) } noexcept;
        requires concepts::NothrowCollectionElement<std::invoke_result_t<const std::decay_t<Step>&>>;
    };

/** Guarded `cycle` admission: a cljonic source whose element type is
 *  nothrow-storable. */
template <typename Source>
concept ValidCycleSource =
    concepts::CljonicSource<Source> && concepts::NothrowCollectionElement<std::ranges::range_value_t<const Source>>;

} // namespace concepts_detail

} // namespace cljonic
