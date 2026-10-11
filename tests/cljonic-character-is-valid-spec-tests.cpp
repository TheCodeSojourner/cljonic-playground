#include <catch2/catch_test_macros.hpp>

#include <limits>
#include <string_view>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

static_assert(cljonic::concepts_detail::CharacterValidityInput<char>);
static_assert(cljonic::concepts_detail::CharacterValidityInput<int>);
static_assert(cljonic::concepts_detail::CharacterValidityInput<unsigned char>);
static_assert(!cljonic::concepts_detail::CharacterValidityInput<bool>);
static_assert(!cljonic::concepts_detail::CharacterValidityInput<double>);
static_assert(!cljonic::concepts_detail::CharacterValidityInput<std::string_view>);
static_assert(cljonic::concepts_detail::CharacterStringInput<const char (&)[2]>);
static_assert(cljonic::concepts_detail::CharacterStringInput<char (&)[2]>);
static_assert(!cljonic::concepts_detail::CharacterStringInput<const char*>);
static_assert(!cljonic::concepts_detail::CharacterStringInput<int>);
static_assert(!cljonic::concepts_detail::CharacterStringInput<std::string_view>);

} // namespace

TEST_CASE("String character validity predicate", "[character_is_valid]") {
    using cljonic::character_is_valid;
    using cljonic::String;

    TRACE_ID("entity-fields.CharacterIsValid");
    TRACE_ID("invariant.CharacterIsValid.CharacterIsValidFreeFunctionSupported");
    TRACE_ID("invariant.CharacterIsValid.CStringCharacterIsValidFreeFunctionSupported");
    TRACE_ID("invariant.CharacterIsValid.StringStaticCharacterPredicateSupported");
    TRACE_ID("invariant.CharacterIsValid.AcceptsNonboolIntegralArguments");
    TRACE_ID("invariant.CharacterIsValid.AcceptsOneCharacterCString");
    TRACE_ID("invariant.CharacterIsValid.RequiresCStringNullTerminator");
    TRACE_ID("invariant.CharacterIsValid.RejectsCStringsOfOtherExtents");
    TRACE_ID("invariant.CharacterIsValid.RejectsUnsupportedArgumentTypes");
    TRACE_ID("invariant.CharacterIsValid.ChecksCharRepresentabilityBeforeConversion");
    TRACE_ID("invariant.CharacterIsValid.ReturnsFalseForOutOfRangeValues");
    TRACE_ID("invariant.CharacterIsValid.RejectsNulCharacter");
    TRACE_ID("invariant.CharacterIsValid.RejectsUnsignedByteAbove7f");
    TRACE_ID("invariant.CharacterIsValid.HasTargetedRejectionDiagnostic");
    TRACE_ID("invariant.CharacterIsValid.HasCStringExtentRejectionDiagnostic");
    TRACE_ID("invariant.CharacterIsValid.RejectionFallbackIsNotSupported");
    TRACE_ID("invariant.CharacterIsValid.HasTargetedRejectionDiagnostic");
    TRACE_ID("invariant.CharacterIsValid.RejectionFallbackIsNotSupported");
    TRACE_ID("invariant.CharacterIsValid.ConstexprExecutionModel");
    TRACE_ID("invariant.CharacterIsValid.NoexceptExecutionModel");
    TRACE_ID("invariant.CharacterIsValid.NonAllocatingSemantics");
    TRACE_ID("invariant.String.HasPublicCharacterValidityMethod");

    STATIC_REQUIRE(String<4>::character_is_valid('A'));
    STATIC_REQUIRE(noexcept(String<4>::character_is_valid('A')));
    STATIC_REQUIRE_FALSE(String<4>::character_is_valid('\0'));
    STATIC_REQUIRE_FALSE(String<4>::character_is_valid(static_cast<char>(0x80)));
    STATIC_REQUIRE(character_is_valid('A'));
    STATIC_REQUIRE(noexcept(character_is_valid('A')));
    STATIC_REQUIRE(character_is_valid(65));
    STATIC_REQUIRE(character_is_valid(71));
    STATIC_REQUIRE_FALSE(character_is_valid(std::numeric_limits<char>::max() + 1));
    STATIC_REQUIRE_FALSE(character_is_valid(300));
    STATIC_REQUIRE_FALSE(character_is_valid('\0'));
    STATIC_REQUIRE_FALSE(character_is_valid(static_cast<char>(0x80)));
    STATIC_REQUIRE_FALSE(character_is_valid(static_cast<unsigned char>(0x80)));

    STATIC_REQUIRE(character_is_valid("A"));
    STATIC_REQUIRE(character_is_valid("x"));
    STATIC_REQUIRE(noexcept(character_is_valid("A")));
    STATIC_REQUIRE_FALSE(character_is_valid("\0"));

    static constexpr char nul_terminated_character[2] = {'B', '\0'};
    static constexpr char unterminated_character[2] = {'B', 'C'};
    STATIC_REQUIRE(character_is_valid(nul_terminated_character));
    STATIC_REQUIRE_FALSE(character_is_valid(unterminated_character));
}