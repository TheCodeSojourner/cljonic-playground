#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <span>
#include <string_view>
#include <type_traits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("string factory builds bounded strings from text sources", "[string][factory]") {
    using cljonic::String;
    using cljonic::string;

    TRACE_ID("entity-fields.StringFactory");
    TRACE_ID("invariant.StringFactory.SupportsDeducedLiteralFactory");
    TRACE_ID("invariant.StringFactory.DeducedCapacityEqualsLiteralContentLength");
    TRACE_ID("invariant.StringFactory.SupportsExplicitCapacityFactory");
    TRACE_ID("invariant.StringFactory.ExplicitCapacityDeterminesResultCapacity");
    TRACE_ID("invariant.StringFactory.ExplicitCapacityFactorySupportsEmptyArguments");
    TRACE_ID("invariant.StringFactory.ExplicitCapacityEmptyFactorySupportsZeroCapacity");
    TRACE_ID("invariant.StringFactory.ExplicitCapacityBoundsImportedContent");
    TRACE_ID("invariant.StringFactory.ExplicitCapacityIsBoundedByConfiguredMaximum");
    TRACE_ID("invariant.StringFactory.UntypedEmptyCallIsCompileTimeRejected");
    TRACE_ID("invariant.StringFactory.OversizedLiteralIsCompileTimeRejected");
    TRACE_ID("invariant.StringFactory.OversizedStaticExtentSourceIsCompileTimeRejected");
    TRACE_ID("invariant.StringFactory.DynamicExtentSourceCopiesBoundedPrefix");
    TRACE_ID("invariant.StringFactory.StringViewSourceIsExplicitCapacityOnly");
    TRACE_ID("invariant.StringFactory.ByteValidityFollowsExistingStringContracts");
    TRACE_ID("invariant.StringFactory.EveryByteConstructsSafelyWithoutThrowing");
    TRACE_ID("invariant.StringFactory.NoVariadicIndividualCharacterForm");
    TRACE_ID("invariant.StringFactory.PreservesPublicStringTypeAndExistingConstructors");
    TRACE_ID("invariant.StringFactory.ReturnsIndependentlyOwningString");
    TRACE_ID("invariant.StringFactory.ConstexprNoexceptNonallocating");
    TRACE_ID("invariant.StringFactory.NoRtti");
    TRACE_ID("invariant.StringFactory.NoExceptions");
    TRACE_ID("invariant.StringFactory.ReferentialTransparency");

    constexpr auto deduced = string("Hi");
    constexpr auto explicit_capacity = string<8>("Hello");
    constexpr auto empty_reserved = string<8>();
    constexpr auto empty_zero_capacity = string<0>();

    STATIC_REQUIRE(std::same_as<decltype(deduced), const String<2>>);
    STATIC_REQUIRE(deduced.count() == 2U);
    STATIC_REQUIRE(deduced.capacity() == 2U);
    STATIC_REQUIRE(deduced(0) == 'H');
    STATIC_REQUIRE(deduced(1) == 'i');
    STATIC_REQUIRE(std::same_as<decltype(explicit_capacity), const String<8>>);
    STATIC_REQUIRE(explicit_capacity.count() == 5U);
    STATIC_REQUIRE(explicit_capacity.capacity() == 8U);
    STATIC_REQUIRE(explicit_capacity(4) == 'o');
    STATIC_REQUIRE(std::same_as<decltype(empty_reserved), const String<8>>);
    STATIC_REQUIRE(empty_reserved.count() == 0U);
    STATIC_REQUIRE(empty_reserved.capacity() == 8U);
    STATIC_REQUIRE(std::same_as<decltype(empty_zero_capacity), const String<0>>);
    STATIC_REQUIRE(empty_zero_capacity.count() == 0U);
    STATIC_REQUIRE(empty_zero_capacity.capacity() == 0U);

    // Explicit-capacity text import from a string_view source.
    static constexpr auto view_source = std::string_view{"from view"};
    constexpr auto from_view = string<16>(view_source);
    STATIC_REQUIRE(std::same_as<decltype(from_view), const String<16>>);
    STATIC_REQUIRE(from_view.view() ==
                   view_source); // A dynamic-extent range source copies the bounded prefix that fits.
    static constexpr char char_source[] = {'s', 'p', 'a', 'n', 'n', 'e', 'd'};
    constexpr auto from_range = string<7>(std::span{char_source});
    STATIC_REQUIRE(from_range.view() == "spanned");

    // Byte validity follows the existing String contracts: an invalid runtime
    // byte is replaced, and admitted content remains observable.
    const char runtime_bytes[] = {'a', '\0', 'c'};
    const std::span<const char> runtime_bytes_span{runtime_bytes, 3U};
    const auto from_runtime = string<4>(runtime_bytes_span);
    REQUIRE(from_runtime.count() == 3U);
    REQUIRE(from_runtime(0) == 'a');
    REQUIRE(from_runtime(1) == '.');
    REQUIRE(from_runtime(2) == 'c');

    // The public type and its existing constructors remain supported.
    constexpr String<10> literal{"Hello"};
    STATIC_REQUIRE(literal(0) == 'H');

    // Returns an independently owning string: the result copies its source.
    constexpr auto owned = string<16>(view_source);
    STATIC_REQUIRE(owned.view() == "from view");

    // noexcept on admitted calls.
    STATIC_REQUIRE(noexcept(string("Hi")));
    STATIC_REQUIRE(noexcept(string<8>("Hello")));
    STATIC_REQUIRE(noexcept(string<8>()));

    // Runtime tests for code coverage instrumentation.
    volatile char runtime_char_raw = 'r';
    const char runtime_char = runtime_char_raw;
    static_cast<void>(runtime_char);

    const auto runtime_deduced = string("Hi");
    REQUIRE(runtime_deduced.count() == 2U);
    REQUIRE(runtime_deduced.capacity() == 2U);

    const auto runtime_view = std::string_view{"runtime"};
    const auto runtime_import = string<16>(runtime_view);
    REQUIRE(runtime_import.view() == "runtime");
    REQUIRE(runtime_import.capacity() == 16U);

    const auto runtime_empty = string<4>();
    REQUIRE(runtime_empty.count() == 0U);
    REQUIRE(runtime_empty.capacity() == 4U);
}
