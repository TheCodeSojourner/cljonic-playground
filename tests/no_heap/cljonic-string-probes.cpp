#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <array>
#include <ranges>
#include <span>
#include <string_view>
#include <type_traits>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto string_probe() noexcept -> bool {
    const auto s = String<8>{"Hello"};
    const auto can_append = s.can_conj('!');
    const auto appended = s.conj('!');
    const char static_source[] = {'A', 'B'};
    const auto static_imported = String<2>{std::span<const char, 2>{static_source}};
    const char source[] = {'W', 'o', 'r', 'l', 'd'};
    const auto imported = String<3>{std::views::all(source)};

    // Factory coverage: deduced literal, explicit capacity, empty, and text
    // import through the public string free function.
    const char factory_source[] = {'a', 'b', 'c'};
    const std::span<const char> factory_span{factory_source};
    const auto deduced = string("Hi");
    const auto reserved = string<8>("Hello");
    const auto empty = string<8>();
    const auto zero = string<0>();
    const auto view_import = string<8>(std::string_view{"runtime"});
    const auto range_import = string<8>(factory_span);

    // Runtime-derived source prevents constant folding from masking the
    // factory's runtime text import.
    volatile char runtime_byte_raw = 'z';
    const char runtime_byte = runtime_byte_raw;
    const std::array runtime_source{runtime_byte, 'y'};
    const auto runtime_import = string<4>(std::span<const char>{runtime_source});

    using Deduced = std::remove_cvref_t<decltype(deduced)>;
    using Reserved = std::remove_cvref_t<decltype(reserved)>;
    using Empty = std::remove_cvref_t<decltype(empty)>;
    using Zero = std::remove_cvref_t<decltype(zero)>;
    using ViewImport = std::remove_cvref_t<decltype(view_import)>;
    using RangeImport = std::remove_cvref_t<decltype(range_import)>;
    using RuntimeImport = std::remove_cvref_t<decltype(runtime_import)>;

    return s.count() == 5U && s(0) == 'H' && can_append && appended.count() == 6U && appended(5) == '!' &&
           imported.view() == "Wor" && static_imported.view() == "AB" && deduced.view() == "Hi" &&
           Deduced::capacity() == 2U && reserved.count() == 5U && Reserved::capacity() == 8U && empty.count() == 0U &&
           Empty::capacity() == 8U && zero.count() == 0U && Zero::capacity() == 0U && view_import.view() == "runtime" &&
           ViewImport::capacity() == 8U && range_import.view() == "abc" && RangeImport::capacity() == 8U &&
           runtime_import.view() == "zy" && RuntimeImport::capacity() == 4U;
}

} // namespace cljonic::no_heap::probes