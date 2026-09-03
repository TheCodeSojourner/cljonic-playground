# cljonic Requirements - Module 4: Sequence Producers & Materialization Pipeline

## Purpose and Scope

This module specifies sequence producer types (`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`), materialization mechanisms (`into`, `fits_into`), and bounded C++ interoperability inputs (`std::span`, `std::string_view`). Module 4 bridges explicit sequence generators to concrete stored collections without dynamic allocation or hidden caching. Standard ranges and views MAY be used internally by cljonic free-function implementations when they preserve the requirements in this module and MUST NOT be exposed as public cljonic result types. Direct use of standard ranges and views by cljonic applications is outside this library contract.

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

REQ-SEQ-021. Operations that produce sequenceable results SHOULD return an owning bounded cljonic value when a useful finite result capacity can be derived from the operation and its inputs. Operations whose results are unbounded, or for which no useful finite capacity can be derived without imposing an arbitrary caller-independent limit, SHOULD return or expose an explicit producer value instead. A runtime-varying result count alone MUST NOT require producer semantics when a safe compile-time capacity bound exists. Producer values MUST be self-contained values that own their parameters, MUST NOT own materialized result storage, MUST NOT borrow source storage or hidden mutable state, and MUST NOT hide an arbitrary result-capacity limit; complete materialization MUST use an explicit bounded destination through `into`, with `fits_into` providing the completeness preflight where required.

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

## Internal Range and View Implementation Support

REQ-PLAT-017. All supported collection types (`Vector`, `Map`, `Set`, `Queue`, and `String`) MUST provide a const range-compatible logical traversal mechanism sufficient for cljonic free-function implementations to visit each active element without depending on collection-specific storage details. The traversal mechanism MUST be non-mutating, non-allocating, non-throwing, and `constexpr`-capable where supported; MUST expose only active elements; and MUST preserve each collection's documented logical traversal order. It MUST provide the equivalent of const `begin`/`end` traversal, but the exact iterator and sentinel types remain an implementation detail unless required by a public interoperability contract.

REQ-PLAT-017A. For each supported collection type `C`, const traversal MUST be usable from a const collection expression, MUST expose a read-only dereference result, MUST advance without throwing or allocating, and MUST terminate at an end sentinel after exactly `C.count()` active elements. Traversal of a `Queue` MUST enumerate elements from front to back in FIFO order regardless of physical storage layout. Traversal of a `String` MUST exclude its null terminator. Traversal of a `Map` MUST expose value-semantic `MapEntry` elements, and traversal of a `Set` MUST expose its stored values.

REQ-PLAT-018. Every supported collection MUST provide an explicitly identified, const-qualified, read-only C++ interoperability accessor or equivalent const logical-range interoperability mechanism. A collection MAY provide `std::span<const T>`-like access only when its active logical elements are represented as one contiguous range; a `String` MAY provide `std::string_view`-like access for its stored content excluding its null terminator. These accessors or equivalent mechanisms MUST be non-allocating, non-throwing, `constexpr`-capable where supported, MUST expose only active logical content, and MUST NOT provide mutable references, mutable iterators, or mutable views into collection storage.

REQ-PLAT-019. cljonic free-function implementations MAY use `std::ranges` algorithms and `std::views` for bounded internal traversal, selection, transformation, or reduction. Such use MUST remain contained within the operation and MUST preserve cljonic's bounded, deterministic, no-heap, no-exception, input-preservation, and owning-result contracts.

REQ-PLAT-020. Public cljonic semantic free functions MUST NOT return standard ranges, standard views, borrowed iterators, or other lazy or non-owning traversal results. A traversal-based operation MUST return its documented owning cljonic value, scalar value, predicate result, checked result, or producer value. This prohibition does not apply to the explicitly identified collection-owned C++ interoperability accessors governed by `REQ-PLAT-018`, `REQ-PLAT-021`, and `REQ-PLAT-022`.

REQ-PLAT-021. C++ interoperability accessors MUST be const-qualified observers returning non-owning, read-only observations whose validity extends only while the source collection remains alive and unmodified. They MUST NOT extend the source lifetime, retain hidden state, or provide a public mutable path to collection storage. The read-only guarantee applies to the returned element or character access, not merely to the accessor's member-function qualifier.

REQ-PLAT-022. C++ interoperability accessors, when provided, MUST expose the active logical range in the collection's documented representation: vector, set, and queue elements; map entries; and string content excluding its null terminator. A queue accessor MUST preserve logical FIFO order. A queue with wrapped circular storage MUST NOT expose a single contiguous `std::span<const T>` unless it first provides a representation that makes the complete active logical range contiguous without allocating or mutating the source. Map/set accessors MUST NOT imply an ordered semantic contract where none is specified.

REQ-PLAT-022A. `Queue` MUST provide its const range-compatible logical traversal as the interoperability mechanism for FIFO access. `Queue` MUST NOT be required to provide a single contiguous standard view when its active logical range is physically wrapped. A queue interoperability accessor MAY return a single `std::span<const T>` only when the complete active logical range is already contiguous; it MUST NOT allocate, copy into a temporary buffer, mutate, or normalize the queue solely to manufacture such a span. A wrapped queue MAY expose no contiguous standard-view accessor.

REQ-PLAT-023. Direct use of `std::ranges`, `std::views`, or collection interoperability accessors by a cljonic user application is an application design decision outside the cljonic requirements. cljonic MUST document only the collection interoperability and traversal support required by its own public operations.

## Traceability and Related Requirements

- **Downstream Artifact**: `Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly` producer templates, `into`, `fits_into`, and internal collection traversal support.
- **Governed REQs**: `REQ-VAL-014`–`017`, `REQ-SEQ-015`–`021`, `REQ-FN-009`–`014C`, `REQ-FN-027`, `REQ-PLAT-017`–`023`, including `REQ-PLAT-017A` and `REQ-PLAT-022A`.
