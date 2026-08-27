# cljonic Architecture - Module 1: Foundation & Nominal Type System

## Purpose and Scope

This document specifies the implementation architecture for Module 1 of `cljonic`. It translates the foundation, memory allocation, value semantics, and nominal type recognition requirements (`cljonic-requirements-module-1.md`) into C++23 structural patterns, traits machinery, and static assertions.

## Closed Nominal Collection Domain Architecture

The nominal collection domain is strictly closed to:
- `cljonic::Vector<T, N>`
- `cljonic::Map<K, V, N>`
- `cljonic::Set<T, N>`
- `cljonic::Queue<T, N>`
- `cljonic::String<N>`

External containers, standard containers (`std::vector`, `std::string`), or third-party classes are strictly prohibited from nominal admission.

## Two-Level Type Model: Level 1 (Nominal Identity Gate)

Nominal admission is enforced prior to any structural capability inspection:

```cpp
namespace cljonic::detail {

enum class collection_kind {
    vector,
    map,
    set,
    queue,
    string
};

template<class Type>
struct collection_traits {
    static constexpr bool is_collection = false;
};

template<class Type>
inline constexpr bool is_cljonic_collection_v =
    collection_traits<std::remove_cvref_t<Type>>::is_collection;

template<class Type>
inline constexpr collection_kind collection_kind_of_v =
    collection_traits<std::remove_cvref_t<Type>>::kind;

} // namespace cljonic::detail
```

### Public Nominal Concepts

```cpp
template<class Type>
concept cljonic_collection =
    detail::is_cljonic_collection_v<Type>;

template<class Type>
concept cljonic_vector =
    cljonic_collection<Type> &&
    (detail::collection_kind_of_v<Type> == detail::collection_kind::vector);

template<class Type>
concept cljonic_map =
    cljonic_collection<Type> &&
    (detail::collection_kind_of_v<Type> == detail::collection_kind::map);

template<class Type>
concept cljonic_set =
    cljonic_collection<Type> &&
    (detail::collection_kind_of_v<Type> == detail::collection_kind::set);

template<class Type>
concept cljonic_queue =
    cljonic_collection<Type> &&
    (detail::collection_kind_of_v<Type> == detail::collection_kind::queue);

template<class Type>
concept cljonic_string =
    cljonic_collection<Type> &&
    (detail::collection_kind_of_v<Type> == detail::collection_kind::string);
```

## Memory & Storage Layout Architecture

1. **Fixed-Capacity Automatic/Static Storage**:
   - Every collection type contains an internal fixed-size buffer (e.g., `std::array<T, N>` or uninitialized byte storage buffer) owned directly by value.
   - Dynamic memory allocation (`new`, `malloc`, standard library container allocation) is strictly forbidden in all paths.

2. **No Exceptions & No RTTI**:
   - No `throw`, `try`, or `catch` keywords.
   - No `typeid`, `dynamic_cast`, or virtual method tables (`vptr`).

3. **Persistent Value Semantics via Deep Copying**:
   - Collection update operations (append, associate, disjoin, pop) return new owning collection instances by deep-copying stored elements.
   - Structural sharing (tree node sharing, reference counting) is NOT used.

## Traceability

- Governed Requirements: `cljonic-requirements-module-1.md` (`REQ-PLAT-001`–`010`, `REQ-PLAT-012`–`013`, `REQ-VAL-001`–`013`, `REQ-VAL-018`–`020`).
- Downstream Modules: Module 2 (Capability Concepts), Module 3 (Concrete Collection Implementations).
