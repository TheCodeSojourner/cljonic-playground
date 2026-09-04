#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

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
    using value_type = int;

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return true;
    }
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

struct MapLike {
    using key_type = int;
    using lookup_type = int;
    using mapped_type = int;
    using value_type = int;

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return true;
    }
    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
    [[nodiscard]] constexpr auto operator()(const int&) const noexcept -> int {
        return 0;
    }
    [[nodiscard]] constexpr auto contains(const int&) const noexcept -> bool {
        return false;
    }
};

struct SetLookupKey {};

struct SetLike {
    using value_type = int;
    using lookup_type = SetLookupKey;

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return true;
    }
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

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return true;
    }
    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
};

struct StringLike {
    using value_type = char;

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return true;
    }
    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return 0;
    }
};

// Structurally identical to VectorLike but NOT admitted to the cljonic domain
// (no cljonic-owned trait specialization). Used to verify nominal admission
// excludes structural similarity.
struct ExternalLike {
    using value_type = int;

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return true;
    }
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

// Admitted collection using size()/empty() instead of count()/is_empty().
// Verifies the capability surface requires count()/is_empty() members.
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

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return true;
    }
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

// The nominal admission concept is a constexpr noexcept predicate; evaluating
// it requires no heap, no RTTI, no exceptions, no threads, and is referentially
// transparent.
template <typename T>
[[nodiscard]] constexpr auto is_admitted() noexcept -> bool {
    return cljonic::concepts::CljonicCollection<T>;
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
    TRACE_ID("invariant.SequenceableCollection.RequiresNonThrowingIsEmpty");
    TRACE_ID("invariant.SequenceableCollection.RequiresNonThrowingCount");
    TRACE_ID("invariant.SequenceableCollection.LayeredOnNominalAdmission");

    // RequiresNonThrowingIsEmpty + RequiresNonThrowingCount.
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
    TRACE_ID("invariant.IndexedCollection.ExtendsSequenceable");
    TRACE_ID("invariant.IndexedCollection.RequiresCallableIndexedLookup");
    TRACE_ID("invariant.IndexedCollection.RequiresContainsPredicate");

    // ExtendsSequenceable: an indexed collection is sequenceable.
    STATIC_REQUIRE(SequenceableCollection<VectorLike>);
    STATIC_REQUIRE(IndexedCollection<VectorLike>);
    STATIC_REQUIRE(IndexedCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(IndexedCollection<cljonic::String<8>>);

    // RequiresCallableIndexedLookup + RequiresContainsPredicate: a sequenceable
    // collection without operator()(size_t)/contains is not indexed.
    STATIC_REQUIRE(SequenceableCollection<SetLike>);
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
    TRACE_ID("invariant.LookupCollection.ExtendsSequenceable");
    TRACE_ID("invariant.LookupCollection.RequiresLookupType");
    TRACE_ID("invariant.LookupCollection.RequiresCallableLookup");
    TRACE_ID("invariant.LookupCollection.RequiresMembershipPredicate");

    STATIC_REQUIRE(LookupCollection<MapLike>);
    STATIC_REQUIRE(LookupCollection<SetLike>);
    STATIC_REQUIRE(LookupCollection<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE(LookupCollection<cljonic::Set<int, 4>>);
    STATIC_REQUIRE_FALSE(LookupCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE_FALSE(LookupCollection<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE_FALSE(LookupCollection<ExternalLike>);
}

// ============================================================================
// AssociativeCollection
// ============================================================================

TEST_CASE("AssociativeCollection structural capability", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.AssociativeCollection");
    TRACE_ID("invariant.AssociativeCollection.ExtendsSequenceable");
    TRACE_ID("invariant.AssociativeCollection.RequiresCallableKeyLookup");
    TRACE_ID("invariant.AssociativeCollection.RequiresMembershipPredicate");

    // ExtendsSequenceable.
    STATIC_REQUIRE(SequenceableCollection<MapLike>);
    STATIC_REQUIRE(AssociativeCollection<MapLike>);
    STATIC_REQUIRE(AssociativeCollection<cljonic::Map<int, int, 4>>);

    // RequiresCallableKeyLookup + RequiresMembershipPredicate: a sequenceable
    // collection without key lookup/contains is not associative.
    STATIC_REQUIRE(SequenceableCollection<SetLike>);
    STATIC_REQUIRE_FALSE(AssociativeCollection<SetLike>);
    STATIC_REQUIRE_FALSE(AssociativeCollection<cljonic::Set<int, 4>>);
    STATIC_REQUIRE_FALSE(AssociativeCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE_FALSE(AssociativeCollection<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE_FALSE(AssociativeCollection<cljonic::String<8>>);
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

// ============================================================================
// ConceptMemberNaming
// ============================================================================

TEST_CASE("ConceptMemberNaming surface", "[concepts][collection]") {
    using namespace cljonic::concepts;

    TRACE_ID("entity-fields.ConceptMemberNaming");
    TRACE_ID("invariant.ConceptMemberNaming.UsesCountIsEmptyMembers");
    TRACE_ID("invariant.ConceptMemberNaming.OmitsIndexBracketLookup");
    TRACE_ID("invariant.ConceptMemberNaming.UsesContainsPredicate");

    // UsesCountIsEmptyMembers: count()/is_empty() are required; size()/empty()
    // alone do not satisfy the sequenceable surface.
    STATIC_REQUIRE(SequenceableCollection<VectorLike>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Vector<int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Set<int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::Queue<int, 4>>);
    STATIC_REQUIRE(SequenceableCollection<cljonic::String<8>>);
    STATIC_REQUIRE_FALSE(SequenceableCollection<SizeEmptyLike>);

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

    STATIC_REQUIRE(cljonic::concepts::VectorElement<ThrowingDefault>);
    STATIC_REQUIRE(cljonic::concepts::VectorElement<ThrowingAssignment>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowVectorElement<ThrowingDefault>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowVectorElement<ThrowingAssignment>);
    STATIC_REQUIRE(cljonic::concepts::NothrowVectorElement<int>);
    STATIC_REQUIRE(noexcept(cljonic::Vector<int, 4>{1, 2}));
}

TEST_CASE("Element construction requires non-throwing value copies", "[concepts]") {
    struct ThrowingCopy {
        ThrowingCopy() noexcept = default;
        ThrowingCopy(const ThrowingCopy&) noexcept(false) {
        }
        operator int() const noexcept {
            return 1;
        }
    } argument;

    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowElementConstruction<int, decltype(argument)>);
}
