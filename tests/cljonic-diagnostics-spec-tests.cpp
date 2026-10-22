#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

// A type that is not a NothrowCollectionElement (its copy constructor is
// deleted), used to exercise the repeat factory's diagnostic fallback.
struct NonStorable {
    NonStorable() = default;
    NonStorable(const NonStorable&) = delete;
};

// The internal class-template diagnostic specialization exposes no usable
// element state, so it carries no element type alias.
template <typename T>
concept HasElementTypeAlias = requires { typename T::value_type; };

// A zero-argument step used to exercise the Repeatedly guidance redirect.
struct GenerateStep {
    constexpr auto operator()() const noexcept -> int {
        return 7;
    }
};

} // namespace

TEST_CASE("RejectionDiagnostic targeted compile-time diagnostics", "[diagnostics]") {
    using cljonic::equal;
    using cljonic::get;
    using cljonic::not_equal;

    TRACE_ID("entity-fields.RejectionDiagnostic");
    TRACE_ID("invariant.RejectionDiagnostic.PrimaryAdmissionRemainsConceptBased");
    TRACE_ID("invariant.RejectionDiagnostic.DiagnosticOverloadPerSupportedArity");
    TRACE_ID("invariant.RejectionDiagnostic.ConstrainedOnNegationOfAdmissionGate");
    TRACE_ID("invariant.RejectionDiagnostic.DetectionUsesNamedConceptsNotCallability");
    TRACE_ID("invariant.RejectionDiagnostic.AppliesToEqualAndNotEqual");
    TRACE_ID("invariant.RejectionDiagnostic.AppliesToProducerFactoryFunctions");
    TRACE_ID("invariant.RejectionDiagnostic.AppliesToCollectionPrimitives");

    // PrimaryAdmissionRemainsConceptBased: supported operands are admitted by
    // the named capability concepts, and valid calls resolve to the real
    // overloads (no diagnostic fires).
    STATIC_REQUIRE(cljonic::concepts_detail::EqualPairAdmissible<int, int>);
    STATIC_REQUIRE(cljonic::concepts_detail::EqualPairAdmissible<cljonic::Vector<int, 2>, cljonic::Range<int>>);
    STATIC_REQUIRE(equal(1, 1));
    STATIC_REQUIRE(not_equal(1, 2));

    // DiagnosticOverloadPerSupportedArity: an out-of-domain operand now has a
    // viable diagnostic overload in every arity. The call expression is
    // well-formed; the targeted static_assert fires at instantiation, which is
    // proved by scripts/check-equal-compile-failures.py.
    STATIC_REQUIRE(requires { equal(1.0); });
    STATIC_REQUIRE(requires { equal(1.0, 1.0); });
    STATIC_REQUIRE(requires { equal(1.0, 1.0, 1.0); });

    // ConstrainedOnNegationOfAdmissionGate: the fallback is constrained on the
    // negation of the admission gate for the same operand pair.
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::EqualPairAdmissible<double, double>);

    // DetectionUsesNamedConceptsNotCallability: domain support is decided by the
    // named admission concepts, not by callability.
    STATIC_REQUIRE_FALSE(cljonic::concepts::StableEqualityComparable<double>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::EqualPairAdmissible<double, double>);

    // AppliesToEqualAndNotEqual: not_equal mirrors equal's domain and fallback.
    STATIC_REQUIRE(requires { not_equal(1.0); });
    STATIC_REQUIRE(requires { not_equal(1.0, 1.0); });
    STATIC_REQUIRE(requires { not_equal(1.0, 1.0, 1.0); });

    // AppliesToProducerFactoryFunctions: the producer factories repeat, cycle,
    // iterate, and repeatedly expose a diagnostic fallback for an out-of-domain
    // argument, so the rejected call expression is still well-formed and the
    // targeted static_assert fires at instantiation (proved by
    // scripts/check-producer-compile-failures.py).
    STATIC_REQUIRE(requires { cljonic::cycle(42); });
    STATIC_REQUIRE(requires { cljonic::repeatedly(42); });
    STATIC_REQUIRE(requires { cljonic::repeatedly(3, 42); });
    STATIC_REQUIRE(requires { cljonic::iterate(42, 1); });
    STATIC_REQUIRE(requires { cljonic::repeat(NonStorable{}); });
    STATIC_REQUIRE(requires { cljonic::repeat(3U, NonStorable{}); });

    // AppliesToCollectionPrimitives: collection primitives expose diagnostic
    // fallbacks for rejected domains. The targeted compile-fail harness checks
    // get's collection/key/fallback cases and dissoc's Map and exact-key cases.
    STATIC_REQUIRE_FALSE(cljonic::concepts::AssociativeCollection<cljonic::Set<int, 4>>);
    STATIC_REQUIRE(requires { cljonic::assoc(cljonic::Set<int, 4>{}, 1, 1); });
    STATIC_REQUIRE(requires { cljonic::assoc(cljonic::Map<int, int, 4>{}, 1, 100, 2); });
    STATIC_REQUIRE(requires { get(42, 0); });
    STATIC_REQUIRE(requires { get(cljonic::Vector<int, 4>{}, 0.5); });
    STATIC_REQUIRE(requires { get(cljonic::Vector<int, 4>{}, 0, 1.5); });
    STATIC_REQUIRE(requires { cljonic::dissoc(cljonic::Set<int, 4>{}); });
    STATIC_REQUIRE(requires { cljonic::dissoc(cljonic::Set<int, 4>{}, 1); });
    STATIC_REQUIRE(requires { cljonic::dissoc(cljonic::Set<int, 4>{}, 1, 2); });
    STATIC_REQUIRE(requires { cljonic::dissoc(cljonic::Map<int, int, 4>{}, 1, short{2}); });
}

TEST_CASE("ClassTemplateDeductionDiagnostic redirects out-of-domain deduction", "[diagnostics]") {
    using cljonic::Cycle;
    using cljonic::Iterate;
    using cljonic::Range;
    using cljonic::Repeat;
    using cljonic::Repeatedly;
    using cljonic::Variant;
    using cljonic::concepts::RangeElement;
    using cljonic::concepts_detail::RangeElementDomainViolation;

    TRACE_ID("entity-fields.ClassTemplateDeductionDiagnostic");
    TRACE_ID("invariant.ClassTemplateDeductionDiagnostic.DomainGateUsesNamedCapabilityConcept");
    TRACE_ID("invariant.ClassTemplateDeductionDiagnostic.OutOfDomainDeductionRedirectsToDiagnosticSpecialization");
    TRACE_ID("invariant.ClassTemplateDeductionDiagnostic.RedirectGuideProvidedPerSupportedArity");
    TRACE_ID("invariant.ClassTemplateDeductionDiagnostic.DiagnosticSpecializationInternalAndUnsupported");
    TRACE_ID("invariant.ClassTemplateDeductionDiagnostic.AdmittedDeductionUnchanged");
    TRACE_ID("invariant.ClassTemplateDeductionDiagnostic.NonDeducibleTypeArgumentRedirectsToGuidance");

    // DomainGateUsesNamedCapabilityConcept: the element domain is a named
    // capability concept, so the type-level gate still rejects softly rather
    // than becoming ill-formed.
    STATIC_REQUIRE(RangeElement<int>);
    STATIC_REQUIRE(RangeElement<long>);
    STATIC_REQUIRE_FALSE(RangeElement<double>);
    STATIC_REQUIRE_FALSE(RangeElement<unsigned>);
    STATIC_REQUIRE_FALSE(RangeElement<const char*>);

    // OutOfDomainDeductionRedirectsToDiagnosticSpecialization: an out-of-domain
    // deduction selects the internal diagnostic specialization instead of
    // failing as a candidate list. The static_assert fires only when the
    // specialization is actually constructed, so deduction is inspectable here.
    STATIC_REQUIRE(std::same_as<decltype(Range{"Hello"}), Range<RangeElementDomainViolation>>);

    // RedirectGuideProvidedPerSupportedArity: one redirecting guide per
    // supported arity, including a mixed-argument arity.
    STATIC_REQUIRE(std::same_as<decltype(Range{1.5}), Range<RangeElementDomainViolation>>);
    STATIC_REQUIRE(std::same_as<decltype(Range{1, 5.0}), Range<RangeElementDomainViolation>>);
    STATIC_REQUIRE(std::same_as<decltype(Range{1, 5, 2.0}), Range<RangeElementDomainViolation>>);

    // DiagnosticSpecializationInternalAndUnsupported: the specialization
    // exposes no usable element state.
    STATIC_REQUIRE_FALSE(HasElementTypeAlias<Range<RangeElementDomainViolation>>);

    // AdmittedDeductionUnchanged: every in-domain deduction is untouched.
    STATIC_REQUIRE(std::same_as<decltype(Range{5}), Range<int>>);
    STATIC_REQUIRE(std::same_as<decltype(Range{2, 5}), Range<int>>);
    STATIC_REQUIRE(std::same_as<decltype(Range{1, 5, 2}), Range<int>>);
    STATIC_REQUIRE(std::same_as<decltype(Range{1L, 5L}), Range<long>>);

    // The same redirect covers every other class-template-gated producer:
    // Repeat (element and count arities), Cycle (source), and Iterate (initial
    // value and step). Each rejected deduction selects the internal diagnostic
    // specialization instead of failing as a candidate list.
    STATIC_REQUIRE(
        std::same_as<decltype(Repeat{"Hello"}), Repeat<cljonic::concepts_detail::RepeatElementDomainViolation>>);
    STATIC_REQUIRE(
        std::same_as<decltype(Repeat{3U, "Hello"}), Repeat<cljonic::concepts_detail::RepeatElementDomainViolation>>);
    STATIC_REQUIRE(std::same_as<decltype(Cycle{"Hello"}), Cycle<cljonic::concepts_detail::CycleSourceDomainViolation>>);
    STATIC_REQUIRE(
        std::same_as<decltype(Iterate{1, "Hello"}), Iterate<cljonic::concepts_detail::IterateDomainViolation,
                                                            cljonic::concepts_detail::IterateDomainViolation>>);

    // NonDeducibleTypeArgumentRedirectsToGuidance: a Variant's alternatives and
    // a Repeatedly's element type are never deducible from a constructor
    // argument, so every such deduction is redirected to a guidance message.
    STATIC_REQUIRE(std::same_as<decltype(cljonic::Variant{1}),
                                cljonic::Variant<cljonic::concepts_detail::VariantDomainViolation>>);
    STATIC_REQUIRE(std::same_as<decltype(cljonic::Repeatedly{GenerateStep{}}),
                                cljonic::Repeatedly<cljonic::concepts_detail::RepeatedlyDomainViolation,
                                                    cljonic::concepts_detail::RepeatedlyDomainViolation>>);

    // Their explicit forms are untouched.
    STATIC_REQUIRE(std::same_as<decltype(Repeat{7}), Repeat<int>>);
    STATIC_REQUIRE(std::same_as<decltype(Repeat{3U, 7}), Repeat<int>>);
    STATIC_REQUIRE(std::same_as<decltype(Cycle{cljonic::Vector<int, 3>{1, 2, 3}}), Cycle<cljonic::Vector<int, 3>>>);
    STATIC_REQUIRE(std::same_as<decltype(cljonic::Variant<int, long>{1}), cljonic::Variant<int, long>>);
}
