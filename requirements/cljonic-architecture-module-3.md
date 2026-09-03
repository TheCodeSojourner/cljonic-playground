# cljonic Architecture - Module 3: Core Collection Types & Primitive Free Functions

## Purpose and Scope

This document specifies the implementation architecture for Module 3 of `cljonic`. It translates the concrete stored collection requirements (`cljonic-requirements-module-3.md`) into C++ class templates, contiguous storage strategies, linear scan lookup algorithms, and free-function primitives.

## Collection Concrete Storage Architecture

All five stored collection types use flat, contiguous, array-backed internal storage:

```cpp
namespace cljonic {

template<class T, std::size_t N>
class Vector {
    static_assert(N <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT, "Capacity exceeds maximum");
private:
    std::array<T, N> m_elements{};
    std::size_t m_count{0};
public:
    // ...
};

template<class K, class V>
struct MapEntry {
    K key;
    V value;
};

template<class K, class V, std::size_t N>
class Map {
private:
    std::array<MapEntry<K, V>, N> m_entries{};
    std::size_t m_count{0};
public:
    // ...
};

template<class T, std::size_t N>
class Set {
private:
    std::array<T, N> m_elements{};
    std::size_t m_count{0};
public:
    // ...
};

template<class T, std::size_t N>
class Queue {
private:
    std::array<T, N> m_elements{};
    std::size_t m_count{0};
public:
    // ...
};

template<std::size_t N>
class String {
private:
    std::array<char, N + 1> m_buffer{}; // N content bytes + 1 null terminator
    std::size_t m_count{0};
public:
    // ...
};

} // namespace cljonic
```

## Linear Scan & Swap-and-Remove Architecture

1. **Unordered Map & Set Search**:
   - `Map` and `Set` lookup, insertion, and removal use **bounded linear scans** (`O(N)`). Hash tables, trees, and dynamic buckets are not used.
2. **Swap-and-Remove for Unsorted Disjoin/Dissoc**:
   - On removal from `Set` or `Map`, the target element is replaced by copying the last active element into its position, and the count is decremented in `O(1)`.
3. **Queue FIFO & Vector Order Preservation**:
   - `Queue` removal pops from front (shifting elements or ring indexing).
   - `Vector` preserves exact insertion index order.

## Callable Collection Lookup Architecture

Collection types overload `operator()` to provide convenience lookup:
- `vector(idx)` / `vector(idx, fallback)` $\rightarrow$ delegates to `get(vector, idx)`
- `map(key)` / `map(key, fallback)` $\rightarrow$ delegates to `get(map, key)`
- `set(val)` / `set(val, fallback)` $\rightarrow$ delegates to `get(set, val)`

`operator[]` is explicitly omitted to prevent accidental unchecked access or insertion syntax.

## Primitive Free Function Layer Architecture

Primitive operations are exposed via free-function templates:
- `count(c)`, `empty(c)`, `is_empty(c)`, `not_empty(c)`
- `first(c)`, `next(c)`, `rest(c)`, `seq(c)`
- `get(c, key/idx, fallback)`, `conj(c, item)`, `assoc(m, k, v)`, `dissoc(m, k)`, `disj(s, v)`, `peek(q)`, `pop(q)`

## Traceability

- Governed Requirements: `cljonic-requirements-module-3.md` (`REQ-COLL-001`–`017`, `REQ-SEQ-001`–`014`, `REQ-SEQ-002A`–`002B`, `REQ-FN-001`–`008A`, `REQ-FN-026`).
- Downstream Modules: Module 4 (Producers & Materialization), Module 5 (Algorithms).
