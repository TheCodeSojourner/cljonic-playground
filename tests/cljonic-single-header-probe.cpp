#include <cljonic.hpp>

int main() {
  constexpr cljonic::Vector<int, 4> values{10, 20};
  static_assert(cljonic::count(values) == 2);
  static_assert(cljonic::get(values, 1U) == 20);
  return cljonic::get(values, 2U, 99) == 99 ? 0 : 1;
}
