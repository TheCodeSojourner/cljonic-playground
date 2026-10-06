#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

// A type that is not a NothrowCollectionElement (its copy constructor is
// deleted), used to exercise the repeat factory's diagnostic fallback.
struct NonStorable {
    NonStorable() = default;
    NonStorable(const NonStorable&) = delete;
};

} // namespace

TEST_CASE("RejectionDiagnostic targeted compile-time diagnostics", "[diagnostics]") {
    using cljonic::equal;
    using cljonic::not_equal;

    TRACE_ID("entity-fields.RejectionDiagnostic");
    TRACE_ID("invariant.RejectionDiagnostic.PrimaryAdmissionRemainsConceptBased");
    TRACE_ID("invariant.RejectionDiagnostic.DiagnosticOverloadPerSupportedArity");
    TRACE_ID("invariant.RejectionDiagnostic.ConstrainedOnNegationOfAdmissionGate");
    TRACE_ID("invariant.RejectionDiagnostic.DetectionUsesNamedConceptsNotCallability");
    TRACE_ID("invariant.RejectionDiagnostic.AppliesToEqualAndNotEqual");
    TRACE_ID("invariant.RejectionDiagnostic.AppliesToProducerFactoryFunctions");

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
    STATIC_REQUIRE(requires { cljonic::repeat(NonStorable{}, 3); });
}
