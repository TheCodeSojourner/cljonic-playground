#include <cljonic.hpp>

int main() {
    constexpr cljonic::Vector<int, 4> values{10, 20};
    static_assert(values.count() == 2);
    static_assert(values(1U) == 20);
    return values(2U) == 0 ? 0 : 1;
}
