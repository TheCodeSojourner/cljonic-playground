💡 Catch2 generator parametrization pattern for state/boundary testing

**Pattern:** Use `GENERATE` + structured bindings to eliminate repetitive SECTION blocks when testing the same function across multiple independent parameter combinations.

**Example:** Vector state classification across (capacity, size) pairs:
```cpp
auto [capacity, size, expected_state] = GENERATE(
  std::make_tuple(1, 0, vector_state::empty),
  std::make_tuple(2, 1, vector_state::populated),
  std::make_tuple(4, 4, vector_state::at_capacity),
  // ... more tuples
);

CHECK(classify_vector(vector_observation{capacity, size}) == expected_state);
```

**Benefits:**
- Eliminates repetitive SECTION boilerplate
- Scales easily: add tuples without code duplication
- Automatic naming per parameter combo in test output
- Clear coverage matrix visibility (output shows "N assertions")

**When to use:**
- Boundary/state testing where multiple parameter combos → same behavior
- Matrix coverage (e.g., different capacities × different fill levels)
- Any parametric test without control-flow differences per case

**Limitation:** Cannot combine incompatible types in one tuple (e.g., `Vector<int, 1>` vs `Vector<int, 2>`).

**Workaround when types differ:**
1. Test via inline cases:
   ```cpp
   auto [vec] = std::pair{Vector{1}, 1U};
   CHECK(vec.state() == vector_state::at_capacity);
   ```

2. Or use if-chain with GENERATE on scalar parameter only:
   ```cpp
   auto [fill_count] = GENERATE(std::make_tuple(1), std::make_tuple(2));
   if (fill_count == 1) {
     Vector<int, 4> v{1};
     CHECK(...);
   } else {
     Vector<int, 4> v{1, 2};
     CHECK(...);
   }
   ```

**Application in cljonic:**
- Refactored vector state classification: 3 SECTIONs → 1 GENERATE with 11 cases
- Construction contract parametrized fill levels: 1 case → 3 cases
- Ready for future set/map/range boundary testing
