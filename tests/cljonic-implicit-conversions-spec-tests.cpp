#include <cstddef>
#include <iterator>
#include <limits>
#include <ranges>
#include <span>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {
struct ConvertsToInt {
    constexpr operator int() const noexcept {
        return 1;
    }
};
enum class ConversionEnum { one = 1 };

struct CopyPreservingValue {
    int value{};
    constexpr CopyPreservingValue() noexcept = default;
    constexpr explicit CopyPreservingValue(int input) noexcept : value(input) {
    }
    constexpr CopyPreservingValue(const CopyPreservingValue&) noexcept = default;
    constexpr CopyPreservingValue(CopyPreservingValue&&) noexcept(false) : value(-1) {
    }
    constexpr auto operator=(const CopyPreservingValue&) noexcept -> CopyPreservingValue& = default;
    constexpr auto operator==(const CopyPreservingValue&) const noexcept -> bool = default;
};

struct CopyPreservingStep {
    int value{};
    constexpr CopyPreservingStep() noexcept = default;
    constexpr explicit CopyPreservingStep(int input) noexcept : value(input) {
    }
    constexpr CopyPreservingStep(const CopyPreservingStep&) noexcept = default;
    constexpr CopyPreservingStep(CopyPreservingStep&&) noexcept(false) : value(-1) {
    }
    constexpr auto operator()() const noexcept -> int {
        return value;
    }
};

template <typename Reference>
struct ActualReferenceIterator {
    using value_type = double;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;
    Reference* current{};
    constexpr auto operator*() const noexcept -> Reference& {
        return *current;
    }
    constexpr auto operator++() noexcept -> ActualReferenceIterator& {
        ++current;
        return *this;
    }
    constexpr void operator++(int) noexcept {
        ++current;
    }
    constexpr auto operator==(const ActualReferenceIterator&) const noexcept -> bool = default;
};

template <typename Reference>
struct ActualReferenceRange {
    Reference values[2];
    constexpr auto begin() noexcept -> ActualReferenceIterator<Reference> {
        return {values};
    }
    constexpr auto end() noexcept -> ActualReferenceIterator<Reference> {
        return {values + 2};
    }
};
} // namespace

TEST_CASE("Implicit conversion admission preserves values and semantic domains", "[conversions]") {
    using cljonic::concepts::SafeImplicitConversion;
    TRACE_ID("entity-fields.ImplicitConversions");
    TRACE_ID("invariant.ImplicitConversions.SameTypeValuesPreserveIdentity");
    TRACE_ID("invariant.ImplicitConversions.ArithmeticConversionsPreserveEntireSourceDomain");
    TRACE_ID("invariant.ImplicitConversions.BooleanCharacterEnumDomainsRemainDistinct");
    TRACE_ID("invariant.ImplicitConversions.RejectsCrossTypeUserDefinedAndPointerConversions");

    STATIC_REQUIRE(SafeImplicitConversion<int, const int&>);
    STATIC_REQUIRE(SafeImplicitConversion<long long, short>);
    STATIC_REQUIRE(SafeImplicitConversion<double, float>);
    STATIC_REQUIRE(SafeImplicitConversion<double, short>);
    STATIC_REQUIRE(cljonic::concepts::NothrowElementConstruction<double, short>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<short, int>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<unsigned int, int>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<int, unsigned int>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<int, double>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<float, double>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<int, bool>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<int, char>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<char, int>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<int, ConversionEnum>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<int, ConvertsToInt>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<const int*, int*>);
    STATIC_REQUIRE(SafeImplicitConversion<bool, bool>);
    STATIC_REQUIRE(SafeImplicitConversion<char, char>);
    STATIC_REQUIRE(SafeImplicitConversion<ConversionEnum, ConversionEnum>);
    STATIC_REQUIRE(SafeImplicitConversion<ConvertsToInt, ConvertsToInt>);
    STATIC_REQUIRE_FALSE(SafeImplicitConversion<int, int()>);
}

TEST_CASE("Same-type producer and Variant arguments preserve nonthrowing copy semantics", "[conversions]") {
    using namespace cljonic;
    TRACE_ID("invariant.ImplicitConversions.ArgumentConstructionGateIsDistinctFromCollectionStorageAdmission");
    TRACE_ID("invariant.ImplicitConversions.ArgumentConstructionGateRequiresSafeConversionAndNothrowConstruction");
    TRACE_ID("invariant.ImplicitConversions.ConstructionChecksActualArgumentExpressionPreservingCvref");
    TRACE_ID("invariant.ImplicitConversions.SafeConversionAloneDoesNotProveNothrowConstruction");
    STATIC_REQUIRE(concepts::NothrowCollectionElement<CopyPreservingValue>);
    STATIC_REQUIRE(concepts::SafeImplicitConversion<CopyPreservingValue, CopyPreservingValue>);
    STATIC_REQUIRE(concepts::NothrowElementConstruction<CopyPreservingValue, const CopyPreservingValue&>);
    STATIC_REQUIRE_FALSE(concepts::NothrowElementConstruction<CopyPreservingValue, CopyPreservingValue>);
    STATIC_REQUIRE_FALSE(concepts::NothrowElementConstruction<CopyPreservingValue, volatile CopyPreservingValue&>);
    volatile int raw = 7;
    const CopyPreservingValue value{raw};
    const auto repeated = repeat(CopyPreservingValue{raw}, 2U);
    CHECK((*repeated.begin()).value == 7);
    const auto cycled = cycle(Vector<CopyPreservingValue, 1>{value});
    CHECK((*cycled.begin()).value == 7);
    const auto generated = repeatedly(2U, CopyPreservingStep{raw});
    CHECK(*generated.begin() == 7);
    const auto iterated = iterate([](const CopyPreservingValue& item) noexcept { return item; }, value);
    auto position = iterated.begin();
    CHECK((*position).value == 7);
    ++position;
    CHECK((*position).value == 7);
    const Variant<CopyPreservingValue, int> expected{value};
    const Variant<CopyPreservingValue, int> actual{CopyPreservingValue{raw}};
    CHECK(equal(actual, expected));
}

TEST_CASE("Safe conversions agree across construction lookup update and removal", "[conversions]") {
    using namespace cljonic;
    TRACE_ID("invariant.ImplicitConversions.AllApiBoundariesShareAdmission");
    constexpr short key = 7;
    constexpr short value = 12;
    constexpr MapEntry<long long, long long> entry{key, value};
    constexpr Map<long long, long long, 4> map{entry};
    STATIC_REQUIRE(contains(map, key));
    STATIC_REQUIRE(get(map, key, short{-1}) == value);
    STATIC_REQUIRE(can_assoc(map, key, value));
    STATIC_REQUIRE(get(assoc(map, key, short{13}), key) == 13);
    STATIC_REQUIRE(is_empty(dissoc(map, key)));
    constexpr Set<long long, 4> set{key};
    STATIC_REQUIRE(contains(set, key));
    STATIC_REQUIRE(is_empty(disj(set, key)));
    constexpr Vector<long long, 4> vector{value};
    STATIC_REQUIRE(get(conj(vector, key), 1) == key);
    STATIC_REQUIRE(can_conj(Queue<long long, 4>{value}, key));
    STATIC_REQUIRE(Repeat<long long>{value, 2U}.count() == 2U);
    STATIC_REQUIRE(Range<long long>{short{0}, short{3}}.count() == 3U);
    STATIC_REQUIRE(Vector<double, 2>{value}(0) == 12.0);
    STATIC_REQUIRE(Queue<double, 2>{value}.peek() == 12.0);
    STATIC_REQUIRE(MapEntry<int, double>{7, value}.value == 12.0);
    STATIC_REQUIRE(MapEntry<int, double>{7}.value == 0.0);
    STATIC_REQUIRE_FALSE(std::is_convertible_v<int, MapEntry<int, double>>);

    volatile short raw = 42;
    const short runtime_value = raw;
    const auto updated = assoc(map, runtime_value, runtime_value);
    CHECK(contains(updated, runtime_value));
    CHECK(get(updated, runtime_value) == 42);
    CHECK_FALSE(contains(dissoc(updated, runtime_value), runtime_value));
    CHECK(is_empty(disj(Set<long long, 2>{runtime_value}, runtime_value)));
    CHECK(get(conj(Vector<double, 2>{}, runtime_value), 0) == 42.0);
    CHECK(Vector<double, 2>{runtime_value}(0) == 42.0);
    CHECK(Queue<double, 2>{runtime_value}.peek() == 42.0);
    CHECK(MapEntry<int, double>{1, runtime_value}.value == 42.0);
    CHECK(MapEntry<int, double>{1}.value == 0.0);
}

TEST_CASE("Range imports and materialization use safe actual element expressions", "[conversions]") {
    using namespace cljonic;
    TRACE_ID("invariant.ImplicitConversions.RangesCheckActualReferenceAndNothrowTraversal");
    static constexpr short values[] = {1, 2};
    constexpr auto source = std::span{values};
    STATIC_REQUIRE(Vector<long long, 2>{source}(1) == 2);
    STATIC_REQUIRE(Set<long long, 2>{source}.contains(short{2}));
    STATIC_REQUIRE(Queue<long long, 2>{source}.peek() == 1);
    STATIC_REQUIRE(Vector<double, 2>{source}(1) == 2.0);
    STATIC_REQUIRE(Queue<double, 2>{source}.peek() == 1.0);
    STATIC_REQUIRE(fits_into(Vector<long long, 2>{}, Range<short>{short{0}, short{2}}));
    STATIC_REQUIRE(into(Vector<long long, 2>{}, Range<short>{short{0}, short{2}})(1) == 1);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::MaterializableSource<Vector<short, 2>, Range<int>>);
    STATIC_REQUIRE(std::same_as<std::ranges::range_value_t<ActualReferenceRange<short>>, double>);
    STATIC_REQUIRE(std::same_as<std::ranges::range_reference_t<ActualReferenceRange<short>>, short&>);
    STATIC_REQUIRE(concepts::NothrowRangeConstruction<long long, ActualReferenceRange<short>>);
    volatile short raw = 4;
    ActualReferenceRange<short> actual_references{{raw, raw}};
    CHECK(Vector<long long, 2>{actual_references}(1) == 4);
    CHECK(Queue<double, 2>{actual_references}.peek() == 4.0);
    ActualReferenceRange<char> character_references{{static_cast<char>(raw + 93), 'b'}};
    CHECK(String<2>{character_references}(0) == 'a');
}

TEST_CASE("Counts and checked semantic exceptions retain explicit domains", "[conversions]") {
    using namespace cljonic;
    TRACE_ID("invariant.ImplicitConversions.CountsRequireUnsignedNoncharacterIntegerTypes");
    TRACE_ID("invariant.ImplicitConversions.FiniteCountsExcludeBooleanAndCharacterTypes");
    TRACE_ID("invariant.ImplicitConversions.FiniteCountEntireTypeRangeMustFitSizeT");
    TRACE_ID("invariant.ImplicitConversions.UnsupportedCountTypesRejectedWithoutWrappingOrTruncation");
    TRACE_ID("invariant.ImplicitConversions.CheckedIndexAndCharacterExceptionsArePreserved");
    TRACE_ID("invariant.ImplicitConversions.PreservesNominalDomains");
    STATIC_REQUIRE(concepts_detail::ProducerCount<unsigned int>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<int>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<bool>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<unsigned char>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<char>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<signed char>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<wchar_t>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<char8_t>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<char16_t>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<char32_t>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<double>);
    STATIC_REQUIRE_FALSE(concepts_detail::ProducerCount<ConvertsToInt>);
    STATIC_REQUIRE(concepts_detail::ProducerCount<std::size_t>);
    STATIC_REQUIRE(concepts_detail::ProducerCount<unsigned long long> ==
                   (std::numeric_limits<unsigned long long>::digits <= std::numeric_limits<std::size_t>::digits));
    STATIC_REQUIRE(repeat(1, 2U).count() == 2U);
    STATIC_REQUIRE(repeatedly(2U, []() noexcept { return 1; }).count() == 2U);
    constexpr Vector<int, 2> vector{1};
    STATIC_REQUIRE_FALSE(contains(vector, -1));
    STATIC_REQUIRE(get(vector, std::numeric_limits<unsigned long long>::max(), -1) == -1);
    STATIC_REQUIRE(character_is_valid(65));
    STATIC_REQUIRE_FALSE(character_is_valid(256));
    STATIC_REQUIRE(String<2>{"a"}(0) == 'a');
    STATIC_REQUIRE_FALSE(std::is_constructible_v<Variant<int, long>, short>);
    STATIC_REQUIRE_FALSE(std::is_constructible_v<Repeat<short>, int>);
    STATIC_REQUIRE_FALSE(std::is_constructible_v<Range<short>, int>);
    volatile int raw_character = 65;
    const int character = raw_character;
    CHECK(character_is_valid(character));
    CHECK_FALSE(character_is_valid(character + 256));
    CHECK_FALSE(character_is_valid(-character));
    CHECK(character_is_valid(static_cast<unsigned int>(character)));
    CHECK_FALSE(character_is_valid(static_cast<unsigned int>(character + 256)));
    char single_character[] = {static_cast<char>(character), '\0'};
    CHECK(character_is_valid(single_character));
    single_character[1] = 'x';
    CHECK_FALSE(character_is_valid(single_character));
}
