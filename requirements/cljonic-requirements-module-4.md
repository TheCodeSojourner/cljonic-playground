# cljonic Requirements - Module 4: Sequence Producers & Materialization Pipeline

## Purpose and Scope

This module specifies sequence producer types (`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`), materialization mechanisms (`into`, `fits_into`), C++ standard view imports (`std::span`, `std::string_view`), and non-owning observation views (`view(collection)`). Module 4 bridges explicit sequence generators to concrete stored collections without dynamic allocation or hidden caching.

## Materialization & Producer Invariants

REQ-VAL-014. A materialization operation whose maximum possible result cardinality is not statically guaranteed to fit MUST use an explicit bounded destination collection supplied at the call site. Producer values MUST NOT own result storage or require a result-capacity template parameter.

REQ-VAL-015. The destination supplied to `into` MUST encode the result capacity and result collection type. `into` MUST return a new destination-typed collection and MUST leave both the destination value and producer/input values unchanged.

REQ-VAL-016. `count` MUST return the exact element count for a materialized collection and MUST return a conservative maximum materialization count for a producer or composed producer without traversing an unbounded producer.

REQ-VAL-017. Compile-time cardinality composition MUST use saturating arithmetic: every composed producer cardinality MUST be at most `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`.

## Sequence Equality & Deep Traversal Rules

REQ-SEQ-015. Equality between finite sequenceable values MAY compare their elements in sequence order when the element types satisfy deep equality. Equality MUST NOT begin an unbounded traversal.

REQ-SEQ-016. A sequenceable value that is statically known to be unbounded MUST NOT satisfy the default deep-equality capability. Ordinary equality involving such a value MUST fail at compile time rather than execute indefinitely.

REQ-SEQ-017. Deep equality for a nested value MUST require finite-observation capability recursively at every nested level. A finite outer collection MUST NOT acquire deep equality when any nested value requires unbounded traversal for equality.

REQ-SEQ-018. Finite nesting of bounded owning collections and producer values MUST be supported when each value satisfies its storage and capability requirements. The existence of nested producer values MUST NOT cause implicit materialization of those producers.

REQ-SEQ-019. No operation MUST implicitly recursively materialize or traverse an unbounded producer nested inside an owning collection or another producer. Bounded inspection or materialization of nested producers MUST require explicit caller-selected bounds at each materialized level.

REQ-SEQ-020. The library MUST distinguish finite nesting depth from unbounded cardinality. The bounded owning value model MUST NOT require support for genuinely self-referential infinite structural nesting; such support would require a separately approved indirection or lazy-reference capability.

REQ-SEQ-021. Operations that produce sequenceable results SHOULD return an owning bounded cljonic value when a useful finite result capacity can be derived from the operation and its inputs. Operations whose results are unbounded, or for which no useful finite capacity can be derived without imposing an arbitrary caller-independent limit, SHOULD return or expose an explicit producer value instead. A runtime-varying result count alone MUST NOT require producer semantics when a safe compile-time capacity bound exists. Producer values MUST NOT own materialized result storage or hide an arbitrary result-capacity limit; complete materialization MUST use an explicit bounded destination through `into`, with `fits_into` providing the completeness preflight where required.

## Sequence Producer Specifications

REQ-FN-009. `cycle`, `iterate`, `range`, `repeat`, and `repeatedly` MUST be standalone producer values or producer operations that do not own materialized result storage or require a result-capacity template parameter. Their values MUST be materialized through `into` into an explicit bounded destination. Unbounded forms MUST produce at most the destination capacity and MAY return a deterministic prefix; finite forms MUST produce their complete result when it fits the destination.

REQ-FN-010. For `cycle`, `iterate`, `range` without a finite end, `repeat` without a count, and `repeatedly` without a count, complete-result preflight MUST report that the result does not fit unless the operation terminates before reaching its unbounded behavior. Counted `repeat` and `repeatedly`, and finite `range`, MUST use their runtime result count for preflight and bounded `into` materialization.

REQ-FN-011. `Range`, `Repeat`, `Cycle`, `Iterate`, and `Repeatedly` producer values MUST own their parameters and MUST remain valid independently of other values. They MUST NOT allocate result storage or retain borrowed dependencies on source collections, callbacks, or input values.

REQ-FN-012. `range` MUST use an inclusive start and exclusive end, default start `0`, default step `1`, and MUST produce an infinite repetition of `start` when `step` is zero. A nonzero step that moves away from the end MUST produce an empty finite range, and equal start and end MUST produce an empty range when the step is nonzero.

REQ-FN-013. For `range` with both equal start and end and a zero step, zero-step infinite repetition MUST take precedence over empty-range behavior.

REQ-FN-014. Callbacks supplied to `iterate` and `repeatedly` MUST be pure and non-allocating. Counted forms MUST invoke a callback exactly once per produced element during each `into` call when materialized completely; uncounted forms MUST be treated as potentially infinite producers.

REQ-FN-014A. The unbounded-equality restriction MUST apply to every producer form that can produce an unbounded sequence, including open-ended `range`, `cycle`, uncounted `repeat`, uncounted `repeatedly`, and unbounded `iterate`, regardless of whether two such producers currently appear to produce the same values.

REQ-FN-014B. A separate explicitly named structural-comparison operation MAY compare the parameters of unbounded producers, but structural comparison MUST NOT be exposed as ordinary sequence equality and MUST NOT imply that two producers have equal materialized sequences.

REQ-FN-014C. Producer construction MUST NOT evaluate producer callbacks. During each `into` call, an element-producing callback MUST be evaluated as needed for each produced element, and repeated `into` calls over the same producer MUST repeat those callback evaluations rather than reuse a hidden realization cache.

## C++ Import Interoperability and Materialization

REQ-FN-027. `into` and `fits_into` MUST support explicit read-only C++ interop sources in addition to supported cljonic producers. For `std::span<const T>` sources, `into(destination, source)` MUST copy elements into a new collection of the destination's collection type and capacity, and `fits_into(destination, source)` MUST test whether the complete copy fits. For `std::string_view` sources and `String<N>` destinations, `into(destination, source)` MUST copy the source's bytes into a new owning `String<N>`, and `fits_into(destination, source)` MUST test whether the complete source content is valid and fits within `N` content bytes. These overloads MUST leave the source and destination unchanged, MUST not retain the external view, MUST not allocate or throw, and MUST use the existing deterministic bounded-prefix behavior when the complete result does not fit. A runtime `std::string_view` import MUST replace every source byte outside the approved `0x01` through `0x7F` range, including `0x00`, with the ASCII period byte `0x2E` in the returned `String<N>`. The replacement MUST occur before the bounded prefix is selected, so the returned value MUST contain the first `N` converted content bytes when the source has more than `N` bytes. `fits_into(destination, source)` MUST return `false` when any source byte is invalid or when the converted content does not fit; it MUST return `true` only when every source byte is valid and the complete content length is no greater than `N`. During constant evaluation, invalid source bytes and source content longer than `N` MUST produce a compile-time diagnostic rather than replacement or bounded-prefix materialization. `std::string_view` input MUST not require or copy a source null terminator as content; an explicitly included null byte is an invalid source byte under this policy.

## Non-Owning Read-Only Views (`view`)

REQ-PLAT-017. The platform-interoperability capability MUST provide an API that exposes a cljonic `String` as `std::string_view`. Such a view MUST be non-owning, MUST perform no dynamic allocation, MUST expose exactly the string's stored ASCII content excluding its null terminator, and MUST remain valid only while the source `String` object remains alive and unmodified.

REQ-PLAT-018. A `std::string_view` returned for a cljonic `String` MUST NOT be documented as null-terminated, and callers MUST NOT retain it after the source string's lifetime or after an operation that replaces the source string value.

REQ-PLAT-019. The platform-interoperability capability MUST provide the public `view(collection)` free function for each supported collection type: `Vector`, `Map`, `Set`, `Queue`, and `String`. The function MUST return a non-owning, read-only standard view without dynamic allocation or mutation of the source collection.

REQ-PLAT-020. `view(Vector<T, N>)`, `view(Set<T, N>)`, and `view(Queue<T, N>)` MUST return `std::span<const T>`; `view(Map<K, V, N>)` MUST return `std::span<const MapEntry<K, V>>`; and `view(String<N>)` MUST return `std::string_view`.

REQ-PLAT-021. Standard views returned by `view(collection)` MUST be non-owning and read-only, MUST expose only the active logical range, MUST perform no dynamic allocation, and MUST NOT extend the source collection's lifetime.

REQ-PLAT-022. Every collection view MUST remain valid only while its source collection remains alive and unmodified. Copying a view MUST copy only its bounded view state and MUST NOT extend the source collection's lifetime.

REQ-PLAT-023. A view of a map or set MUST preserve the source collection's semantic iteration contract: repeatable implementation order MAY be exposed, but callers MUST NOT rely on any particular order. A view MUST NOT turn an unordered collection into an ordered one.

## Traceability and Related Requirements

- **Downstream Artifact**: `Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly` producer templates, `into`, `fits_into`, and `view` overloads.
- **Governed REQs**: `REQ-VAL-014`–`017`, `REQ-SEQ-015`–`021`, `REQ-FN-009`–`014C`, `REQ-FN-027`, `REQ-PLAT-017`–`023`.
