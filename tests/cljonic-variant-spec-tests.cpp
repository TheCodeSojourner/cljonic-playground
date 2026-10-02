#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <variant>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

// Equality whose comparison is stable but may throw.
struct ThrowingEqualityOnly {
    friend auto operator==(const ThrowingEqualityOnly&, const ThrowingEqualityOnly&) -> bool {
        return true;
    }
};

// A type whose copy construction can throw: not admissible as a Variant
// alternative for storage.
struct ThrowingCopy {
    ThrowingCopy() = default;
    ThrowingCopy(const ThrowingCopy&) {
    }
    ThrowingCopy& operator=(const ThrowingCopy&) = default;
    friend auto operator==(const ThrowingCopy&, const ThrowingCopy&) noexcept -> bool {
        return true;
    }
};

} // namespace

TEST_CASE("cljonic::Variant is the nominal cljonic composite", "[variant][concepts]") {
    using cljonic::Variant;

    TRACE_ID("entity-fields.CljonicVariant");
    TRACE_ID("invariant.CljonicVariant.NominalAdmissionRequiresCljonicOwnedTrait");
    TRACE_ID("invariant.CljonicVariant.DistinctDomainFromStandardLibraryVariant");
    TRACE_ID("invariant.CljonicVariant.HoldsExactlyOneAlternative");
    TRACE_ID("invariant.CljonicVariant.NoValuelessState");
    TRACE_ID("invariant.CljonicVariant.NoThrowingAccessPath");
    TRACE_ID("invariant.CljonicVariant.ConstexprNoexceptNonmutatingNonallocating");
    TRACE_ID("invariant.CljonicVariant.NoRtti");
    TRACE_ID("invariant.CljonicVariant.NestableCompositeComponent");

    // NominalAdmissionRequiresCljonicOwnedTrait / DistinctDomainFromStandardLibraryVariant
    STATIC_REQUIRE(cljonic::concepts_detail::is_cljonic_variant_v<Variant<int, long>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::is_cljonic_variant_v<std::variant<int, long>>);

    // HoldsExactlyOneAlternative: a default-constructed Variant holds the first
    // alternative; index() is always a valid alternative index.
    constexpr auto defaulted = Variant<int, long>{};
    STATIC_REQUIRE(defaulted.index() == 0U);
    STATIC_REQUIRE(defaulted.holds<int>());
    STATIC_REQUIRE(Variant<int, long>{1L}.index() == 1U);
    STATIC_REQUIRE(Variant<int, long>{1L}.holds<long>());
    STATIC_REQUIRE_FALSE(Variant<int, long>{1L}.holds<int>());

    // NoValuelessState: there is no valueless state; every Variant is storable
    // and always holds an active alternative.
    STATIC_REQUIRE(cljonic::concepts::NothrowCollectionElement<Variant<int, long>>);

    // NoThrowingAccessPath: observation cannot throw.
    STATIC_REQUIRE(noexcept(Variant<int, long>{1}.index()));
    STATIC_REQUIRE(noexcept(Variant<int, long>{1}.holds<int>()));

    // ConstexprNoexceptNonmutatingNonallocating: constexpr and non-throwing
    // construction and copy.
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<Variant<int, long>>);
    STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<Variant<int, long>>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Variant<int, long>>);

    // NoRtti: not a polymorphic type.
    STATIC_REQUIRE_FALSE(std::is_polymorphic_v<Variant<int, long>>);

    // NestableCompositeComponent: a Variant nests in another Variant and in a
    // cljonic collection.
    STATIC_REQUIRE(cljonic::concepts::StableEqualityComparable<Variant<Variant<int, long>, char>>);
    STATIC_REQUIRE(cljonic::concepts::StableEqualityComparable<cljonic::Vector<Variant<int, long>, 4>>);
}

TEST_CASE("cljonic::Variant storage admission", "[variant][concepts]") {
    using cljonic::Variant;

    TRACE_ID("entity-fields.VariantStorageAdmission");
    TRACE_ID("invariant.VariantStorageAdmission.EveryAlternativeNothrowCollectionElement");
    TRACE_ID("invariant.VariantStorageAdmission.AdmitsFloatAndCallableAlternativesForStorage");
    TRACE_ID("invariant.VariantStorageAdmission.StorableAsMapValueVectorElementQueueElement");
    TRACE_ID("invariant.VariantStorageAdmission.RejectsNonNothrowStorableAlternative");

    // EveryAlternativeNothrowCollectionElement.
    STATIC_REQUIRE(cljonic::concepts::NothrowVariantAlternative<int>);
    STATIC_REQUIRE(cljonic::concepts::NothrowVariantAlternative<long>);

    // AdmitsFloatAndCallableAlternativesForStorage: float and callable
    // alternatives are storable even though they are not equality-admissible.
    STATIC_REQUIRE(cljonic::concepts::NothrowVariantAlternative<double>);
    STATIC_REQUIRE(cljonic::concepts::NothrowVariantAlternative<int (*)(int)>);
    STATIC_REQUIRE(cljonic::concepts::NothrowCollectionElement<Variant<int, double>>);

    // StorableAsMapValueVectorElementQueueElement.
    STATIC_REQUIRE(requires { typename cljonic::Vector<Variant<int, long>, 2>; });
    STATIC_REQUIRE(requires { typename cljonic::Queue<Variant<int, long>, 2>; });
    STATIC_REQUIRE(requires { typename cljonic::Map<int, Variant<int, long>, 2>; });

    // RejectsNonNothrowStorableAlternative: a throwing copy construction is not
    // admissible as an alternative (enforced by the Variant storage constraint
    // and proved by the variant-compile-fail harness).
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowVariantAlternative<ThrowingCopy>);
}

TEST_CASE("cljonic::Variant equality admission", "[variant][concepts]") {
    using cljonic::Variant;

    TRACE_ID("entity-fields.VariantEqualityAdmission");
    TRACE_ID("invariant.VariantEqualityAdmission.EqualityRequiresAllAlternativesNothrowComparable");
    TRACE_ID("invariant.VariantEqualityAdmission.OmitsEqualityWhenAlternativeNotEqualityAdmissible");
    TRACE_ID("invariant.VariantEqualityAdmission.EqualityPositionUsageRejectedAtCompileTime");
    TRACE_ID("invariant.VariantEqualityAdmission.OrderingRequiresAllAlternativesTotallyOrdered");
    TRACE_ID("invariant.VariantEqualityAdmission.AlternativeStrictEquality");
    TRACE_ID("invariant.VariantEqualityAdmission.NoCrossTypeNumericUnification");
    TRACE_ID("invariant.VariantEqualityAdmission.NoHashBasedEquality");

    // EqualityRequiresAllAlternativesNothrowComparable.
    STATIC_REQUIRE(cljonic::concepts::ComparableVariantAlternative<int>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::ComparableVariantAlternative<ThrowingEqualityOnly>);
    STATIC_REQUIRE(cljonic::concepts::StableEqualityComparable<Variant<int, long>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::StableEqualityComparable<Variant<int, ThrowingEqualityOnly>>);

    // OmitsEqualityWhenAlternativeNotEqualityAdmissible: no equality operator is
    // provided when an alternative is not equality-admissible.
    STATIC_REQUIRE_FALSE(std::equality_comparable<Variant<int, ThrowingEqualityOnly>>);

    // EqualityPositionUsageRejectedAtCompileTime.
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::EqualPairAdmissible<Variant<int, ThrowingEqualityOnly>,
                                                                       Variant<int, ThrowingEqualityOnly>>);

    // OrderingRequiresAllAlternativesTotallyOrdered.
    STATIC_REQUIRE(cljonic::concepts::TotallyOrdered<Variant<int, long>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::TotallyOrdered<Variant<int, ThrowingEqualityOnly>>);
    STATIC_REQUIRE(Variant<int, long>{1} < Variant<int, long>{2});
    STATIC_REQUIRE(Variant<int, long>{1} < Variant<int, long>{1L}); // index order
    STATIC_REQUIRE(Variant<int, long>{2} > Variant<int, long>{1});
    STATIC_REQUIRE(Variant<int, long>{1} <= Variant<int, long>{1});
    STATIC_REQUIRE(Variant<int, long>{1} >= Variant<int, long>{1});
    STATIC_REQUIRE(Variant<int, long>{1} <= Variant<int, long>{2});
    STATIC_REQUIRE(Variant<int, long>{2} >= Variant<int, long>{1});
    STATIC_REQUIRE(Variant<int, long>{2L} < Variant<int, long>{3L}); // long-alternative ordering

    // AlternativeStrictEquality / NoCrossTypeNumericUnification: same
    // alternative compares by value; a different alternative is unequal even
    // when the alternative values compare conventionally equal (1 == 1L).
    STATIC_REQUIRE(Variant<int, long>{1} == Variant<int, long>{1});
    STATIC_REQUIRE_FALSE(Variant<int, long>{1} == Variant<int, long>{1L});
    STATIC_REQUIRE(Variant<int, long>{1} != Variant<int, long>{1L});

    // NoHashBasedEquality: equality is by value, so two independently
    // constructed equal values compare equal (no identity/hash semantics).
    STATIC_REQUIRE(Variant<int, long>{2} == Variant<int, long>{2});

    // Second-alternative paths exercise the recursion and terminal branches of
    // the index walk for both equality and inequality.
    STATIC_REQUIRE(Variant<int, long>{2L} == Variant<int, long>{2L});
    STATIC_REQUIRE(Variant<int, long>{2L} != Variant<int, long>{3L});
}

TEST_CASE("standard-library variant rejection", "[variant][concepts]") {
    using cljonic::Variant;

    TRACE_ID("entity-fields.StandardVariantRejection");
    TRACE_ID("invariant.StandardVariantRejection.StandardLibraryVariantIsNotCljonicValueType");
    TRACE_ID("invariant.StandardVariantRejection.RejectsStandardVariantAsEqualOperand");
    TRACE_ID("invariant.StandardVariantRejection.RejectsStandardVariantAsMapKeyOrSetElement");
    TRACE_ID("invariant.StandardVariantRejection.PermitsStandardVariantAsInternalImplementationDetail");

    // StandardLibraryVariantIsNotCljonicValueType.
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::is_cljonic_variant_v<std::variant<int, long>>);

    // RejectsStandardVariantAsEqualOperand.
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::in_non_cljonic_fallthrough_domain_v<std::variant<int, long>>);
    STATIC_REQUIRE_FALSE(
        cljonic::concepts_detail::EqualPairAdmissible<std::variant<int, long>, std::variant<int, long>>);

    // RejectsStandardVariantAsMapKeyOrSetElement: the standard-library variant
    // is flagged as such and is rejected by the Map/Set admission constraints
    // (proved by the variant-compile-fail harness); an ordinary key is accepted.
    STATIC_REQUIRE(cljonic::concepts_detail::is_std_variant_v<std::variant<int, long>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::is_std_variant_v<int>);
    STATIC_REQUIRE(requires { typename cljonic::Map<int, int, 4>; });
    STATIC_REQUIRE(requires { typename cljonic::Set<int, 4>; });

    // PermitsStandardVariantAsInternalImplementationDetail: cljonic::Variant is
    // a facade over the standard-library variant, so no extra storage is added
    // and the internal use is the only permitted one.
    STATIC_REQUIRE(sizeof(Variant<int, long>) == sizeof(std::variant<int, long>));
}

TEST_CASE("cljonic::Variant runtime alternative paths", "[variant]") {
    using cljonic::Variant;

    // Runtime (non-constexpr) comparisons exercise every active-index branch of
    // the equality and ordering walks, including the second alternative.
    const auto int_a = Variant<int, long>{1};
    const auto int_b = Variant<int, long>{1};
    const auto long_a = Variant<int, long>{2L};
    const auto long_b = Variant<int, long>{2L};
    const auto long_c = Variant<int, long>{3L};

    CHECK(int_a == int_b);
    CHECK(int_a != long_a);
    CHECK(long_a == long_b);
    CHECK(long_a != long_c);
    CHECK(int_a < long_a);
    CHECK(long_a < long_c);
    CHECK(long_a > int_a);
    CHECK(int_a <= int_b);
    CHECK(long_a >= long_b);
}
