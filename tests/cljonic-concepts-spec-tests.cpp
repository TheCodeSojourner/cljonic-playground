#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <deque>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

template <typename C>
concept HasIsEmptyMember = requires(const C& collection) { collection.is_empty(); };

// ============================================================================
// Test-local nominal scaffolding types.
//
// These types opt into the closed cljonic collection domain through the
// cljonic-owned collection_traits specialization (nominal admission), and
// provide the structural members required by the Level 2 capability concepts.
// They test the concepts themselves independently of the concrete containers,
// which conform to the capability surface in Phase C.
// ============================================================================

struct VectorLike {
    using key_type = std::size_t;
    using value_type = int;

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
    [[nodiscard]] constexpr auto operator()(std::size_t) const noexcept -> int {
        return 0;
    }
    [[nodiscard]] constexpr auto contains(std::size_t) const noexcept -> bool {
        return false;
    }
    [[nodiscard]] constexpr auto can_assoc(key_type) const noexcept -> bool {
        return true;
    }
    [[nodiscard]] constexpr auto assoc(key_type, const value_type&) const noexcept -> VectorLike {
        return {};
    }
};

struct MapLike {
    using key_type = int;
    using lookup_type = int;
    using mapped_type = int;
    using association_value_type = int;
    using value_type = int;

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
    [[nodiscard]] constexpr auto operator()(const int&) const noexcept -> int {
        return 0;
    }
    [[nodiscard]] constexpr auto contains(const int&) const noexcept -> bool {
        return false;
    }
    [[nodiscard]] constexpr auto can_assoc(const key_type&) const noexcept -> bool {
        return true;
    }
    [[nodiscard]] constexpr auto assoc(const key_type&, const value_type&) const noexcept -> MapLike {
        return {};
    }
};

struct SetLookupKey {};

struct SetLike {
    using value_type = int;
    using lookup_type = SetLookupKey;

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }

    [[nodiscard]] constexpr auto operator()(const SetLookupKey&) const noexcept -> int {
        return 0;
    }

    [[nodiscard]] constexpr auto contains(const SetLookupKey&) const noexcept -> bool {
        return false;
    }
};

struct QueueLike {
    using value_type = int;

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
};

struct StringLike {
    using key_type = std::size_t;
    using association_value_type = char;
    using value_type = char;

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
    [[nodiscard]] constexpr auto operator()(std::size_t) const noexcept -> char {
        return 0;
    }
    [[nodiscard]] constexpr auto contains(std::size_t) const noexcept -> bool {
        return false;
    }
    [[nodiscard]] constexpr auto can_assoc(key_type) const noexcept -> bool {
        return true;
    }
    [[nodiscard]] constexpr auto assoc(key_type, const value_type&) const noexcept -> StringLike {
        return {};
    }
};

// Structurally identical to VectorLike but NOT admitted to the cljonic domain
// (no cljonic-owned trait specialization). Used to verify nominal admission
// excludes structural similarity.
struct ExternalLike {
    using value_type = int;

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
    [[nodiscard]] constexpr auto operator()(std::size_t) const noexcept -> int {
        return 0;
    }
    [[nodiscard]] constexpr auto contains(std::size_t) const noexcept -> bool {
        return false;
    }
};

// Admitted collection using size()/empty() instead of the required count().
struct SizeEmptyLike {
    using value_type = int;

    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t {
        return 0;
    }
    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return true;
    }
};

// Admitted collection using operator[] instead of operator()(size_t).
// Verifies the indexed surface omits bracket lookup.
struct BracketLike {
    using value_type = int;

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
    [[nodiscard]] constexpr auto operator[](std::size_t) const noexcept -> int {
        return 0;
    }
};

// Equality-only value type (no ordering relation).
struct EqualityOnly {
    friend constexpr auto operator==(const EqualityOnly&, const EqualityOnly&) noexcept -> bool {
        return true;
    }
};

// Equality whose comparison is stable but may throw: admissible to
// StableEqualityComparable, rejected by NothrowEqualityComparable.
struct ThrowingEqualityOnly {
    friend auto operator==(const ThrowingEqualityOnly&, const ThrowingEqualityOnly&) -> bool {
        return true;
    }
};

// The nominal admission concept is a constexpr noexcept predicate; evaluating
// it requires no heap, no RTTI, no exceptions, no threads, and is referentially
// transparent.
template <typename T>
[[nodiscard]] constexpr auto is_admitted() noexcept -> bool {
    return cljonic::concepts::CljonicCollection<T>;
}

// A structurally producer-similar type: it exposes count()/is_finite()/const
// begin()/end() but has no cljonic-owned producer trait specialization, so it
// must NOT be admitted to the producer nominal domain.
struct ExternalProducerLike {
    using value_type = int;

    [[nodiscard]] constexpr auto is_finite() const noexcept -> bool {
        return true;
    }
    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
    [[nodiscard]] constexpr auto begin() const noexcept -> const int* {
        return nullptr;
    }
    [[nodiscard]] constexpr auto end() const noexcept -> const int* {
        return nullptr;
    }
};

// A producer-similar scaffolding type that opts into the producer nominal
// domain through the cljonic-owned producer trait specialization, and provides
// the structural members required by SequenceableProducer.
struct ProducerLike {
    using value_type = int;

    [[nodiscard]] constexpr auto is_finite() const noexcept -> bool {
        return true;
    }
    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
    [[nodiscard]] constexpr auto begin() const noexcept -> const int* {
        return nullptr;
    }
    [[nodiscard]] constexpr auto end() const noexcept -> const int* {
        return nullptr;
    }
};

// The producer nominal admission concept is likewise a constexpr noexcept
// predicate with the same resource properties.
template <typename T>
[[nodiscard]] constexpr auto is_producer_admitted() noexcept -> bool {
    return cljonic::concepts::CljonicProducer<T>;
}

} // namespace

// ============================================================================
// Nominal admission: cljonic-owned trait specializations for the scaffolding
// types. This is the ONLY mechanism that admits a type to the closed cljonic
// collection domain (CljonicCollection.NominalAdmissionRequiresCljonicOwnedTrait).
// ============================================================================

namespace cljonic::concepts_detail {

template <>
struct collection_traits<VectorLike> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::vector;
};

template <>
struct collection_traits<MapLike> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::map;
};

template <>
struct collection_traits<SetLike> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::set;
};

template <>
struct collection_traits<QueueLike> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::queue;
};

template <>
struct collection_traits<StringLike> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::string;
};

template <>
struct collection_traits<SizeEmptyLike> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::vector;
};

template <>
struct collection_traits<BracketLike> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::vector;
};

} // namespace cljonic::concepts_detail

namespace cljonic::concepts_detail {

// Nominal admission for the producer-domain scaffolding type: the ONLY
// mechanism that admits a type to the producer nominal domain
// (CljonicProducer.NominalAdmissionRequiresCljonicOwnedTrait).
template <>
struct producer_traits<ProducerLike> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::range;
};

} // namespace cljonic::concepts_detail

// ============================================================================
// CljonicCollection (nominal admission)
// ============================================================================

TEST_CASE("CljonicCollection nominal admission", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicCollection");
    TRACE_ID("invariant.CljonicCollection.NominalAdmissionRequiresCljonicOwnedTrait");
    TRACE_ID("invariant.CljonicCollection.ExcludesStructuralSimilarity");
    TRACE_ID("invariant.CljonicCollection.DistinguishesCollectionKind");
    TRACE_ID("invariant.CljonicCollection.NoHeapAllocation");
    TRACE_ID("invariant.CljonicCollection.NoRtti");
    TRACE_ID("invariant.CljonicCollection.NoExceptions");
    TRACE_ID("invariant.CljonicCollection.SingleThreadedExecutionModel");
    TRACE_ID("invariant.CljonicCollection.ReferentialTransparency");

    // NominalAdmissionRequiresCljonicOwnedTrait: only cljonic-owned trait
    // specializations admit a type.
    STATIC_REQUIRE(CljonicCollection<VectorLike>);
    STATIC_REQUIRE(CljonicCollection<MapLike>);
    STATIC_REQUIRE(CljonicCollection<SetLike>);
    STATIC_REQUIRE(CljonicCollection<QueueLike>);
    STATIC_REQUIRE(CljonicCollection<StringLike>);

    // ExcludesStructuralSimilarity: a structurally identical type without the
    // cljonic-owned trait specialization is NOT admitted.
    STATIC_REQUIRE_FALSE(CljonicCollection<ExternalLike>);
    STATIC_REQUIRE_FALSE(CljonicCollection<std::vector<int>>);
    STATIC_REQUIRE_FALSE(CljonicCollection<int>);

    // DistinguishesCollectionKind: nominal identity resolves to a distinct kind.
    STATIC_REQUIRE(CljonicVector<VectorLike>);
    STATIC_REQUIRE_FALSE(CljonicVector<MapLike>);

    // NoHeapAllocation, NoRtti, NoExceptions, SingleThreadedExecutionModel,
    // ReferentialTransparency: the nominal admission concept is a constexpr
    // noexcept predicate, so evaluating it requires no heap, no RTTI, no
    // exceptions, no threads, and is referentially transparent.
    STATIC_REQUIRE(is_admitted<VectorLike>());
    STATIC_REQUIRE(noexcept(is_admitted<VectorLike>()));
}

// ============================================================================
// CljonicVector / CljonicMap / CljonicSet / CljonicQueue / CljonicString
// ============================================================================

TEST_CASE("CljonicVector nominal kind identity", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicVector");
    TRACE_ID("invariant.CljonicVector.NominalConceptIdentifiesVectorKind");
    TRACE_ID("invariant.CljonicVector.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicVector<VectorLike>);
    STATIC_REQUIRE(CljonicVector<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE_FALSE(CljonicVector<MapLike>);
    STATIC_REQUIRE_FALSE(CljonicVector<ExternalLike>);
    STATIC_REQUIRE_FALSE(CljonicVector<std::vector<int>>);
}

TEST_CASE("CljonicMap nominal kind identity", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicMap");
    TRACE_ID("invariant.CljonicMap.NominalConceptIdentifiesMapKind");
    TRACE_ID("invariant.CljonicMap.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicMap<MapLike>);
    STATIC_REQUIRE(CljonicMap<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE_FALSE(CljonicMap<VectorLike>);
    STATIC_REQUIRE_FALSE(CljonicMap<ExternalLike>);
    STATIC_REQUIRE_FALSE(CljonicMap<std::map<int, int>>);
}

TEST_CASE("CljonicSet nominal kind identity", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicSet");
    TRACE_ID("invariant.CljonicSet.NominalConceptIdentifiesSetKind");
    TRACE_ID("invariant.CljonicSet.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicSet<SetLike>);
    STATIC_REQUIRE(CljonicSet<cljonic::Set<int, 4>>);
    STATIC_REQUIRE_FALSE(CljonicSet<VectorLike>);
    STATIC_REQUIRE_FALSE(CljonicSet<ExternalLike>);
    STATIC_REQUIRE_FALSE(CljonicSet<std::set<int>>);
}

TEST_CASE("CljonicQueue nominal kind identity", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicQueue");
    TRACE_ID("invariant.CljonicQueue.NominalConceptIdentifiesQueueKind");
    TRACE_ID("invariant.CljonicQueue.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicQueue<QueueLike>);
    STATIC_REQUIRE(CljonicQueue<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE_FALSE(CljonicQueue<VectorLike>);
    STATIC_REQUIRE_FALSE(CljonicQueue<ExternalLike>);
    STATIC_REQUIRE_FALSE(CljonicQueue<std::deque<int>>);
}

TEST_CASE("CljonicString nominal kind identity", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicString");
    TRACE_ID("invariant.CljonicString.NominalConceptIdentifiesStringKind");
    TRACE_ID("invariant.CljonicString.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicString<StringLike>);
    STATIC_REQUIRE(CljonicString<cljonic::String<8>>);
    STATIC_REQUIRE_FALSE(CljonicString<VectorLike>);
    STATIC_REQUIRE_FALSE(CljonicString<ExternalLike>);
    STATIC_REQUIRE_FALSE(CljonicString<std::string>);
}

// ============================================================================
// SequenceableCollection
// ============================================================================

TEST_CASE("SequenceableCollection structural capability", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.SequenceableCollection");
    TRACE_ID("invariant.SequenceableCollection.RequiresNonThrowingCount");
    TRACE_ID("invariant.SequenceableCollection.LayeredOnNominalAdmission");

    // RequiresNonThrowingCount: VectorLike has no is_empty() member.
    STATIC_REQUIRE(SequenceableCollection<VectorLike>);
    STATIC_REQUIRE(SequenceableCollection<MapLike>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Set<int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::String<8>>);

    // LayeredOnNominalAdmission: a structurally identical external type is NOT
    // sequenceable because it is not admitted.
    STATIC_REQUIRE_FALSE(SequenceableCollection<ExternalLike>);
    STATIC_REQUIRE_FALSE(SequenceableCollection<std::vector<int>>);
}

// ============================================================================
// IndexedCollection
// ============================================================================

TEST_CASE("IndexedCollection structural capability", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.IndexedCollection");
    TRACE_ID("invariant.IndexedCollection.SequenceIndependent");
    TRACE_ID("invariant.IndexedCollection.RefinesLookupCapability");
    TRACE_ID("invariant.IndexedCollection.RequiresCallableIndexedLookup");
    TRACE_ID("invariant.IndexedCollection.RequiresContainsPredicate");

    // Sequence independence: indexed access does not require the separate
    // sequence-observation baseline.
    STATIC_REQUIRE(IndexedCollection<VectorLike>);
    STATIC_REQUIRE(IndexedCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(IndexedCollection<cljonic::String<8>>);

    // RequiresCallableIndexedLookup + RequiresContainsPredicate: a nominal
    // collection without operator()(size_t)/contains is not indexed.
    STATIC_REQUIRE_FALSE(IndexedCollection<SetLike>);
    STATIC_REQUIRE_FALSE(IndexedCollection<cljonic::Set<EqualityOnly, 4>>);
    STATIC_REQUIRE_FALSE(IndexedCollection<cljonic::Queue<int, 4>>);

    // operator[] is not sufficient; the surface requires operator()(size_t).
    STATIC_REQUIRE_FALSE(IndexedCollection<BracketLike>);
}

// ============================================================================
// LookupCollection
// ============================================================================

TEST_CASE("LookupCollection general lookup capability", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.LookupCollection");
    TRACE_ID("invariant.LookupCollection.SequenceIndependent");
    TRACE_ID("invariant.LookupCollection.RequiresLookupType");
    TRACE_ID("invariant.LookupCollection.RequiresCallableLookup");
    TRACE_ID("invariant.LookupCollection.RequiresDefaultOrFallbackAccess");
    TRACE_ID("invariant.LookupCollection.RequiresMembershipPredicate");

    STATIC_REQUIRE(LookupCollection<MapLike>);
    STATIC_REQUIRE(LookupCollection<SetLike>);
    STATIC_REQUIRE(LookupCollection<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE(LookupCollection<cljonic::Set<int, 4>>);
    STATIC_REQUIRE(LookupCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(LookupCollection<cljonic::String<8>>);
    STATIC_REQUIRE_FALSE(LookupCollection<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE_FALSE(LookupCollection<ExternalLike>);
}

// ============================================================================
// AssociativeCollection
// ============================================================================

TEST_CASE("AssociativeCollection structural capability", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.AssociativeCollection");
    TRACE_ID("invariant.AssociativeCollection.SequenceIndependent");
    TRACE_ID("invariant.AssociativeCollection.DefinesKeyType");
    TRACE_ID("invariant.AssociativeCollection.DefinesValueType");
    TRACE_ID("invariant.AssociativeCollection.DefinesCapacityPolicy");
    TRACE_ID("invariant.AssociativeCollection.RequiresAssocOperation");
    TRACE_ID("invariant.AssociativeCollection.RequiresCanAssocPreflight");
    TRACE_ID("invariant.AssociativeCollection.ReturnsNewCollectionValue");
    TRACE_ID("invariant.AssociativeCollection.PreservesSource");
    TRACE_ID("invariant.AssociativeCollection.RequiresNonMutatingAssocOperation");
    TRACE_ID("invariant.AssociativeCollection.RequiresNonMutatingAssocPreflight");

    STATIC_REQUIRE(AssociativeCollection<MapLike>);
    STATIC_REQUIRE(AssociativeCollection<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE(AssociativeCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(AssociativeCollection<StringLike>);
    STATIC_REQUIRE(AssociativeCollection<cljonic::String<8>>);

    // Association is independent from callable lookup and membership.
    STATIC_REQUIRE_FALSE(AssociativeCollection<SetLike>);
    STATIC_REQUIRE_FALSE(AssociativeCollection<cljonic::Set<int, 4>>);
    STATIC_REQUIRE_FALSE(AssociativeCollection<cljonic::Queue<int, 4>>);
}

// ============================================================================
// ConjableCollection
// ============================================================================

TEST_CASE("ConjableCollection structural capability", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.ConjableCollection");
    TRACE_ID("invariant.ConjableCollection.SequenceIndependent");
    TRACE_ID("invariant.ConjableCollection.DefinesValueType");
    TRACE_ID("invariant.ConjableCollection.DefinesCapacityPolicy");
    TRACE_ID("invariant.ConjableCollection.RequiresConjOperation");
    TRACE_ID("invariant.ConjableCollection.RequiresCanConjPreflight");
    TRACE_ID("invariant.ConjableCollection.ReturnsNewCollectionValue");
    TRACE_ID("invariant.ConjableCollection.PreservesSource");
    TRACE_ID("invariant.ConjableCollection.RequiresNonMutatingConjOperation");
    TRACE_ID("invariant.ConjableCollection.RequiresNonMutatingConjPreflight");

    STATIC_REQUIRE(ConjableCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(ConjableCollection<cljonic::Set<int, 4>>);
    STATIC_REQUIRE(ConjableCollection<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE(ConjableCollection<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE(ConjableCollection<cljonic::String<8>>);

    // Non-collections are rejected.
    STATIC_REQUIRE_FALSE(ConjableCollection<int>);
}

// ============================================================================
// StableEqualityComparable / TotallyOrdered
// ============================================================================

TEST_CASE("StableEqualityComparable value capability", "[concepts][value]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.StableEqualityComparable");
    TRACE_ID("invariant.StableEqualityComparable.RequiresBoolEquality");
    TRACE_ID("invariant.StableEqualityComparable.RejectsFloatingPoint");

    // RequiresBoolEquality.
    STATIC_REQUIRE(StableEqualityComparable<int>);
    STATIC_REQUIRE(StableEqualityComparable<std::string>);

    // RejectsFloatingPoint: float/double are rejected to prevent NaN/precision
    // instabilities in map keys and set elements.
    STATIC_REQUIRE_FALSE(StableEqualityComparable<float>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<double>);
}

TEST_CASE("TotallyOrdered value capability", "[concepts][value]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.TotallyOrdered");
    TRACE_ID("invariant.TotallyOrdered.ExtendsStableEquality");
    TRACE_ID("invariant.TotallyOrdered.RequiresStrictOrdering");

    // ExtendsStableEquality.
    STATIC_REQUIRE(StableEqualityComparable<int>);

    // RequiresStrictOrdering: a totally ordered type is stable-equality and
    // strictly ordered.
    STATIC_REQUIRE(TotallyOrdered<int>);

    // An equality-only type (no ordering relation) is not totally ordered.
    STATIC_REQUIRE(StableEqualityComparable<EqualityOnly>);
    STATIC_REQUIRE_FALSE(TotallyOrdered<EqualityOnly>);
}

TEST_CASE("NothrowStableEqualityComparable value capability", "[concepts][value]") {
    using namespace cljonic::concepts;

    struct ThrowingEquality {
        ThrowingEquality() noexcept(false) {
        }
        ThrowingEquality(const ThrowingEquality&) noexcept = default;
        ThrowingEquality& operator=(const ThrowingEquality&) noexcept = default;
        bool operator==(const ThrowingEquality&) const noexcept = default;
    };

    TRACE_ID("entity-fields.NothrowStableEqualityComparable");
    TRACE_ID("invariant.NothrowStableEqualityComparable.ExtendsStableEquality");
    TRACE_ID("invariant.NothrowStableEqualityComparable.ExtendsNothrowCollectionElement");

    // ExtendsStableEquality and ExtendsNothrowCollectionElement: the map key
    // and set element admission contract requires both capabilities.
    STATIC_REQUIRE(NothrowStableEqualityComparable<int>);
    STATIC_REQUIRE(NothrowStableEqualityComparable<EqualityOnly>);

    // A type with equality but a throwing storage operation is rejected.
    STATIC_REQUIRE(StableEqualityComparable<ThrowingEquality>);
    STATIC_REQUIRE_FALSE(NothrowCollectionElement<ThrowingEquality>);
    STATIC_REQUIRE_FALSE(NothrowStableEqualityComparable<ThrowingEquality>);

    // A stable-equality type may lack ordering yet still satisfy this concept.
    STATIC_REQUIRE_FALSE(TotallyOrdered<EqualityOnly>);
}

TEST_CASE("NothrowEqualityComparable value capability", "[concepts][value]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.NothrowEqualityComparable");
    TRACE_ID("invariant.NothrowEqualityComparable.ExtendsStableEquality");
    TRACE_ID("invariant.NothrowEqualityComparable.RequiresNothrowComparison");
    TRACE_ID("invariant.NothrowEqualityComparable.RecursesOverVariantAlternatives");

    // ExtendsStableEquality.
    STATIC_REQUIRE(StableEqualityComparable<EqualityOnly>);
    STATIC_REQUIRE(NothrowEqualityComparable<EqualityOnly>);

    // RequiresNothrowComparison: a comparison that may throw is rejected even
    // though it is stable.
    STATIC_REQUIRE(StableEqualityComparable<ThrowingEqualityOnly>);
    STATIC_REQUIRE_FALSE(NothrowEqualityComparable<ThrowingEqualityOnly>);

    // RecursesOverVariantAlternatives: a cljonic::Variant declares its own
    // non-throwing == over its alternatives, so the concept is satisfied by
    // recursion when every alternative compares without throwing.
    STATIC_REQUIRE(NothrowEqualityComparable<cljonic::Variant<int, long>>);
    STATIC_REQUIRE_FALSE(NothrowEqualityComparable<cljonic::Variant<int, ThrowingEqualityOnly>>);
}

// ============================================================================
// Composite stable equality (REQ-CAP-010)
// ============================================================================

TEST_CASE("Composite variant stable equality", "[concepts][value][composite]") {
    using namespace cljonic::concepts;

    TRACE_ID("invariant.StableEqualityComparable.RecursesOverComponents");
    TRACE_ID("invariant.StableEqualityComparable.RejectsCallableComponents");
    TRACE_ID("invariant.AlternativeStrictEquality.RequiresSameAlternative");
    TRACE_ID("invariant.AlternativeStrictEquality.RejectsCrossAlternativeEquality");
    TRACE_ID("invariant.AlternativeStrictEquality.RejectsCrossTypeNumericUnification");

    // RecursesOverComponents: composite admits stable equality when every
    // stored component admits stable equality.
    STATIC_REQUIRE(StableEqualityComparable<cljonic::Variant<int, long>>);
    STATIC_REQUIRE(StableEqualityComparable<cljonic::Variant<int, char, bool>>);

    // Recursion into cljonic collections: a collection with equality components
    // is itself stable-equality comparable (REQ-COLL-021).
    STATIC_REQUIRE(StableEqualityComparable<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(StableEqualityComparable<cljonic::Set<int, 4>>);
    STATIC_REQUIRE(StableEqualityComparable<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE(StableEqualityComparable<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE(StableEqualityComparable<cljonic::String<8>>);
    STATIC_REQUIRE(StableEqualityComparable<cljonic::MapEntry<int, int>>);
    STATIC_REQUIRE(StableEqualityComparable<cljonic::Variant<int, cljonic::Vector<int, 4>>>);

    // RecursesOverComponents: floating-point components are rejected at any
    // nesting level, including inside cljonic collections and nested variants.
    // (Types whose own admission constraints reject floating-point — such as
    // Set<float,4> or MapEntry<double,int> — cannot be formed at all and are
    // verified by the variant-compile-fail gate instead.)
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Variant<int, double>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Variant<int, float>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Variant<cljonic::Variant<int, double>, char>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Vector<double, 4>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Map<int, double, 4>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::MapEntry<int, double>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Variant<int, cljonic::Vector<double, 4>>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Variant<int, cljonic::Map<int, double, 4>>>);

    // RejectsCallableComponents: callable components never admit stable
    // equality, including function pointers (rejected as a class).
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Variant<int, int (*)(int)>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<int (*)(int)>);
}

TEST_CASE("Producer parameter equality stable-recursion", "[concepts][value][producer][composite]") {
    using namespace cljonic::concepts;
    using namespace cljonic;

    TRACE_ID("invariant.StableEqualityComparable.RecursesIntoProducerParameters");
    TRACE_ID("invariant.StableEqualityComparable.RecursesOverComponents");
    TRACE_ID("invariant.StableEqualityComparable.RejectsCallableComponents");

    // RecursesIntoProducerParameters: Range/Repeat/Cycle with all-stable
    // stored parameters admit producer parameter equality.
    STATIC_REQUIRE(StableEqualityComparable<Range<int>>);
    STATIC_REQUIRE(StableEqualityComparable<Repeat<int>>);
    STATIC_REQUIRE(StableEqualityComparable<std::remove_cvref_t<decltype(cycle(Vector<int, 3>{1, 2, 3}))>>);

    // Floating-point stored parameters are rejected at any nesting level,
    // including inside a producer.
    STATIC_REQUIRE_FALSE(StableEqualityComparable<Repeat<double>>);
    STATIC_REQUIRE_FALSE(
        StableEqualityComparable<std::remove_cvref_t<decltype(cycle(Vector<double, 3>{1.0, 2.0, 3.0}))>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Variant<int, Repeat<double>>>);

    // Nested producers admit stable equality only when every nested component
    // admits stable equality (REQ-SEQ-018 / REQ-CAP-010 recursion).
    STATIC_REQUIRE(StableEqualityComparable<Vector<Range<int>, 4>>);
    STATIC_REQUIRE(StableEqualityComparable<Repeat<Vector<int, 3>>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<Vector<Repeat<double>, 4>>);

    // RejectsCallableComponents: Iterate/Repeatedly never admit stable
    // equality because their stored step is a callable component, so they are
    // never usable as a map key or set element.
    STATIC_REQUIRE_FALSE(StableEqualityComparable<Iterate<int, int (*)(int) noexcept>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<Repeatedly<int, int (*)() noexcept>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Variant<int, int (*)(int)>>);

    // A Cycle over a callable-producing source is also rejected.
    STATIC_REQUIRE_FALSE(StableEqualityComparable<std::remove_cvref_t<decltype(cycle(
                             Iterate<int, int (*)(int) noexcept>{0, [](int v) noexcept { return v + 1; }}))>>);
}

TEST_CASE("AlternativeStrictEquality variant-equality semantics", "[value][composite]") {
    TRACE_ID("entity-fields.AlternativeStrictEquality");
    TRACE_ID("invariant.AlternativeStrictEquality.RequiresSameAlternative");
    TRACE_ID("invariant.AlternativeStrictEquality.RejectsCrossAlternativeEquality");
    TRACE_ID("invariant.AlternativeStrictEquality.RejectsCrossTypeNumericUnification");

    using alt = cljonic::Variant<int, long>;

    constexpr alt same_int_one{1};
    constexpr alt same_int_two{1};
    constexpr alt long_one{1L};

    // RequiresSameAlternative: equal alternative values are equal.
    STATIC_REQUIRE(same_int_one == same_int_two);
    STATIC_REQUIRE(long_one == long_one);

    // RejectsCrossAlternativeEquality: a variant holding the long alternative is
    // not equal to a variant holding the int alternative, even though the
    // alternative values compare conventionally equal (1 == 1L).
    STATIC_REQUIRE(same_int_one == same_int_one);
    STATIC_REQUIRE_FALSE(same_int_one == long_one);
    STATIC_REQUIRE(long_one != same_int_one);

    // The variant can be used as a map key / set element under these semantics.
    constexpr cljonic::Map<alt, int, 4> map{cljonic::MapEntry<alt, int>{same_int_one, 10}};
    static_assert(map(alt{1}) == 10);
    static_assert(map(alt{1L}, -1) == -1); // long alternative: not found
    // Runtime coverage of the same lookup paths.
    const auto runtime_map = cljonic::Map<alt, int, 4>{cljonic::MapEntry<alt, int>{alt{1}, 10}};
    CHECK(runtime_map(alt{1}) == 10);
    CHECK(runtime_map(alt{2}, -1) == -1);
    CHECK(runtime_map(alt{1L}, -1) == -1); // cross-alternative miss
    CHECK(runtime_map.contains(alt{1}));
    CHECK_FALSE(runtime_map.contains(alt{1L}));

    const auto runtime_set = cljonic::Set<alt, 4>{alt{1}};
    CHECK(runtime_set.contains(alt{1}));
    CHECK_FALSE(runtime_set.contains(alt{1L}));
    CHECK_FALSE(runtime_set.contains(alt{2}));
}

// ============================================================================
// ConceptMemberNaming
// ============================================================================

TEST_CASE("ConceptMemberNaming surface", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.ConceptMemberNaming");
    TRACE_ID("invariant.ConceptMemberNaming.UsesCountMemberForCardinality");
    TRACE_ID("invariant.ConceptMemberNaming.OmitsIndexBracketLookup");
    TRACE_ID("invariant.ConceptMemberNaming.UsesContainsPredicate");
    TRACE_ID("invariant.IsEmpty.IsEmptyDerivedFromZeroCount");
    TRACE_ID("invariant.IsEmpty.NoCollectionIsEmptyMember");

    // count() is the only required sequence-size member; is_empty is a free function.
    STATIC_REQUIRE(SequenceableCollection<VectorLike>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Set<int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::String<8>>);
    STATIC_REQUIRE_FALSE(SequenceableCollection<SizeEmptyLike>);
    STATIC_REQUIRE_FALSE(HasIsEmptyMember<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE_FALSE(HasIsEmptyMember<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE_FALSE(HasIsEmptyMember<cljonic::Set<int, 4>>);
    STATIC_REQUIRE_FALSE(HasIsEmptyMember<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE_FALSE(HasIsEmptyMember<cljonic::String<8>>);

    // OmitsIndexBracketLookup: operator[] is not used; operator()(size_t) is.
    STATIC_REQUIRE(IndexedCollection<VectorLike>);
    STATIC_REQUIRE(IndexedCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(IndexedCollection<cljonic::String<8>>);
    STATIC_REQUIRE_FALSE(IndexedCollection<BracketLike>);

    // UsesContainsPredicate: the indexed surface requires contains.
    STATIC_REQUIRE(IndexedCollection<VectorLike>);
    STATIC_REQUIRE(IndexedCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(IndexedCollection<cljonic::String<8>>);
}

// ============================================================================
// CljonicProducer (nominal producer admission)
// ============================================================================

TEST_CASE("CljonicProducer nominal producer admission", "[concepts][producer]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicProducer");
    TRACE_ID("invariant.CljonicProducer.NominalAdmissionRequiresCljonicOwnedTrait");
    TRACE_ID("invariant.CljonicProducer.DistinguishesProducerKind");
    TRACE_ID("invariant.CljonicProducer.DistinctDomainFromClosedNominalCollectionDomain");
    TRACE_ID("invariant.CljonicProducer.NoHeapAllocation");
    TRACE_ID("invariant.CljonicProducer.NoRtti");
    TRACE_ID("invariant.CljonicProducer.NoExceptions");
    TRACE_ID("invariant.CljonicProducer.SingleThreadedExecutionModel");
    TRACE_ID("invariant.CljonicProducer.ReferentialTransparency");

    // NominalAdmissionRequiresCljonicOwnedTrait: a structurally similar
    // producer without the cljonic-owned producer trait specialization is NOT
    // admitted, and neither is a non-producer type.
    STATIC_REQUIRE(CljonicProducer<ProducerLike>);
    STATIC_REQUIRE_FALSE(CljonicProducer<ExternalProducerLike>);
    STATIC_REQUIRE_FALSE(CljonicProducer<int>);
    STATIC_REQUIRE_FALSE(CljonicProducer<std::vector<int>>);

    // DistinguishesProducerKind: the family concepts partition the producer
    // domain, so each admitted producer resolves to exactly one family.
    STATIC_REQUIRE(CljonicRange<cljonic::Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicRepeat<cljonic::Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicCycle<cljonic::Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicIterate<cljonic::Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicRepeatedly<cljonic::Range<int>>);

    // DistinctDomainFromClosedNominalCollectionDomain: producers and
    // collections are disjoint nominal domains.
    STATIC_REQUIRE_FALSE(CljonicProducer<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE_FALSE(CljonicCollection<cljonic::Range<int>>);

    // NoHeapAllocation, NoRtti, NoExceptions, SingleThreadedExecutionModel,
    // ReferentialTransparency: the producer nominal admission concept is a
    // constexpr noexcept predicate, so evaluating it requires no heap, no
    // RTTI, no exceptions, no threads, and is referentially transparent.
    STATIC_REQUIRE(is_producer_admitted<ProducerLike>());
    STATIC_REQUIRE(noexcept(is_producer_admitted<ProducerLike>()));
}

// ============================================================================
// CljonicRange / CljonicRepeat / CljonicCycle / CljonicIterate / CljonicRepeatedly
// ============================================================================

TEST_CASE("CljonicRange nominal kind identity", "[concepts][producer]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicRange");
    TRACE_ID("invariant.CljonicRange.NominalConceptIdentifiesRangeKind");
    TRACE_ID("invariant.CljonicRange.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicRange<cljonic::Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicRange<cljonic::Repeat<int>>);
    STATIC_REQUIRE_FALSE(CljonicRange<ExternalProducerLike>);
    STATIC_REQUIRE_FALSE(CljonicRange<std::vector<int>>);
}

TEST_CASE("CljonicRepeat nominal kind identity", "[concepts][producer]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicRepeat");
    TRACE_ID("invariant.CljonicRepeat.NominalConceptIdentifiesRepeatKind");
    TRACE_ID("invariant.CljonicRepeat.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicRepeat<cljonic::Repeat<int>>);
    STATIC_REQUIRE_FALSE(CljonicRepeat<cljonic::Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicRepeat<ExternalProducerLike>);
    STATIC_REQUIRE_FALSE(CljonicRepeat<std::vector<int>>);
}

TEST_CASE("CljonicCycle nominal kind identity", "[concepts][producer]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicCycle");
    TRACE_ID("invariant.CljonicCycle.NominalConceptIdentifiesCycleKind");
    TRACE_ID("invariant.CljonicCycle.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicCycle<cljonic::Cycle<cljonic::Vector<int, 3>>>);
    STATIC_REQUIRE_FALSE(CljonicCycle<cljonic::Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicCycle<ExternalProducerLike>);
    STATIC_REQUIRE_FALSE(CljonicCycle<std::vector<int>>);
}

TEST_CASE("CljonicIterate nominal kind identity", "[concepts][producer]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicIterate");
    TRACE_ID("invariant.CljonicIterate.NominalConceptIdentifiesIterateKind");
    TRACE_ID("invariant.CljonicIterate.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicIterate<cljonic::Iterate<int, int (*)(int) noexcept>>);
    STATIC_REQUIRE_FALSE(CljonicIterate<cljonic::Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicIterate<ExternalProducerLike>);
    STATIC_REQUIRE_FALSE(CljonicIterate<std::vector<int>>);
}

TEST_CASE("CljonicRepeatedly nominal kind identity", "[concepts][producer]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.CljonicRepeatedly");
    TRACE_ID("invariant.CljonicRepeatedly.NominalConceptIdentifiesRepeatedlyKind");
    TRACE_ID("invariant.CljonicRepeatedly.RejectsExternalContainer");

    STATIC_REQUIRE(CljonicRepeatedly<cljonic::Repeatedly<int, int (*)() noexcept>>);
    STATIC_REQUIRE_FALSE(CljonicRepeatedly<cljonic::Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicRepeatedly<ExternalProducerLike>);
    STATIC_REQUIRE_FALSE(CljonicRepeatedly<std::vector<int>>);
}

TEST_CASE("Vector element storage requires non-throwing operations", "[vector][concepts]") {
    struct ThrowingDefault {
        ThrowingDefault() noexcept(false) {
        }
        ThrowingDefault(const ThrowingDefault&) noexcept = default;
        ThrowingDefault& operator=(const ThrowingDefault&) noexcept = default;
    };

    struct ThrowingAssignment {
        ThrowingAssignment() noexcept = default;
        ThrowingAssignment(const ThrowingAssignment&) noexcept = default;
        ThrowingAssignment& operator=(const ThrowingAssignment&) noexcept(false) {
            return *this;
        }
    };

    struct ThrowingDestruction {
        ThrowingDestruction() noexcept = default;
        ThrowingDestruction(const ThrowingDestruction&) noexcept = default;
        ThrowingDestruction& operator=(const ThrowingDestruction&) noexcept = default;
        ~ThrowingDestruction() noexcept(false) {
        }
    };

    STATIC_REQUIRE(cljonic::concepts::CopyableElement<ThrowingDefault>);
    STATIC_REQUIRE(cljonic::concepts::CopyableElement<ThrowingAssignment>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowCollectionElement<ThrowingDefault>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowCollectionElement<ThrowingAssignment>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowCollectionElement<ThrowingDestruction>);
    STATIC_REQUIRE(cljonic::concepts::NothrowCollectionElement<int>);
    STATIC_REQUIRE(noexcept(cljonic::Vector<int, 4>{1, 2}));
}

TEST_CASE("Element construction requires non-throwing value copies", "[concepts]") {
    struct ThrowingCopy {
        ThrowingCopy() noexcept = default;
        ThrowingCopy(const ThrowingCopy&) noexcept(false) {
        }
        operator int() && noexcept(false) {
            return 1;
        }
    } argument;

    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowElementConstruction<int, decltype(argument)>);
}

TEST_CASE("Element construction preserves argument value category", "[concepts]") {
    struct CategorySensitive {
        constexpr operator int() & noexcept {
            return 1;
        }

        constexpr operator int() && noexcept(false) {
            return 1;
        }
    };

    STATIC_REQUIRE(cljonic::concepts::NothrowElementConstruction<int, CategorySensitive&>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowElementConstruction<int, CategorySensitive>);
}
