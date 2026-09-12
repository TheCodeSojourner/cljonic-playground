# cljonic Requirements - Module 6: Numeric & Callable Convenience

## Purpose and Scope

This module specifies the checked fixed-width numeric contract, scalar comparison/selection, bitwise math, conversion/parsing restrictions, raw floating-point policies, closure storage, and callable convenience adapters (`partial`, `juxt`, `fnil`, `apply`, etc.). Module 6 ensures numeric and functional computations remain deterministic, non-allocating, and compile-time checked.

## Fixed-Width Numeric Contract & Policy

REQ-NUM-000. Raw floating-point calculations and comparisons MUST be treated as potentially non-deterministic across values, compiler settings, evaluation modes, and platforms because of rounding, representation, NaN, infinity, signed zero, and other IEEE-754 effects. Raw floating-point behavior MUST NOT be used for a stable semantic contract unless an explicit numeric policy defines and tests that behavior.

REQ-NUM-001. Raw floating-point types MAY be stored as scalar values, vector elements, or map values when the selected operation does not require semantic equality or ordering, because those storage operations do not claim a stable floating-point result or identity contract.

REQ-NUM-002. Raw floating-point types MUST NOT be accepted as map keys or set elements because rounding, representation, NaN, infinity, and signed-zero behavior can make equality unsuitable as a stable identity across supported environments.

REQ-NUM-003. `range` MUST require an integral or otherwise explicitly discrete numeric type. Raw floating-point bounds or steps MUST be rejected at compile time because rounding and evaluation differences can change termination and produced values.

REQ-NUM-004. Any operation that requires equality or ordering MUST express that requirement through a named capability or equivalent compile-time constraint. Raw floating-point types MUST NOT satisfy the default stable-equality or total-order capabilities because their results and comparisons may vary across supported environments.

REQ-NUM-005. Sorting or other ordered traversal of raw floating-point values MUST fail at compile time because NaN, signed zero, rounding, and platform-dependent evaluation can prevent a stable total order. A future explicitly configured total-order numeric wrapper MAY provide the required capability, but raw `float` and `double` MUST remain unsupported by the default policy.

REQ-NUM-006. Equality, distinctness, frequency counting, or other identity operations over raw floating-point values MUST be rejected unless an explicitly approved numeric policy provides the required semantics, because rounding and special-value behavior can make those results non-deterministic. No implicit epsilon comparison or NaN normalization policy is provided.

REQ-NUM-007. Collection operations that compare values MUST apply the same numeric capability constraints recursively to their element, key, and value types. In particular, equality of collections containing raw floating-point values MUST be rejected by default even when the collection itself can store those values, because nested floating-point comparisons may not be stable across supported environments.

REQ-NUM-008. Numeric conversion operations MUST have an explicit target type and MUST NOT select their result type through a global configuration constant. A cljonic equivalent of Clojure `int` MUST return a fixed-width signed 32-bit integer type when the conversion succeeds.

REQ-NUM-009. A default numeric conversion MUST reject at compile time any statically known conversion that loses range, precision, or accuracy. A statically known conversion that is provably exact and representable MAY be accepted in a constant expression.

REQ-NUM-010. When conversion safety cannot be determined at compile time, the library MUST provide an explicitly checked, non-throwing runtime conversion whose result communicates success or failure without dynamic allocation or global mutable error state. The checked result type and failure states MUST be documented before implementation.

REQ-NUM-011. Potentially lossy conversion from a runtime floating-point value to an integer MUST NOT be performed by an implicitly narrowing overload because rounding and representation differences can change the converted result. It MUST require an explicitly checked or explicitly lossy conversion operation named to make the policy visible at the call site.

REQ-NUM-012. Explicitly lossy numeric conversions MAY be provided as separate operations, but their truncation, saturation, wrapping, NaN, infinity, out-of-range, and signedness behavior MUST be documented and tested. They MUST NOT be the default conversion behavior.

REQ-NUM-013. Numeric conversion and text parsing MUST be separate operations. A cljonic equivalent of Clojure `int` MUST accept numeric or character inputs according to its documented source capabilities and MUST NOT implicitly parse a `String`; string-to-integer conversion MUST use a separately named parsing operation with deterministic failure behavior.

REQ-NUM-014. `parse_float` MUST document its accepted syntax, sign, decimal, exponent, infinity, NaN, range, rounding, and accuracy behavior. The parser MUST NOT make raw floating-point parsing suitable for stable equality, ordering, or identity contracts without the explicit numeric policy required elsewhere.

REQ-NUM-015. Fixed-width arithmetic MUST use checked behavior by default. Runtime arithmetic operations MUST provide corresponding preflight predicates such as `can_add`, `can_subtract`, and `can_multiply`; when the predicate is false, the operation MUST return its documented default and leave its inputs unchanged. Compile-time-known arithmetic overflow MUST be rejected during constant evaluation.

REQ-NUM-016. Wrapping and saturating arithmetic, if provided, MUST be separately named operations and MUST NOT be implicit behavior of the default arithmetic operations.

REQ-NUM-017. The first-pass numeric convenience family MUST include checked fixed-width arithmetic corresponding to `add`, `sub`, `mult`, `div`, `quot`, `rem`, and `mod`, with `can_add`, `can_subtract`, and `can_multiply` preflights; scalar comparison and selection corresponding to `compare`, `min`, `max`, `min_key`, and `max_key`; integer-step operations corresponding to `inc`, `dec`, and `negate`; constrained numeric predicates corresponding to `is_zero`, `is_positive`, `is_negative`, `is_even`, and `is_odd`; explicit conversions corresponding to `to_int`, `to_long`, `to_float`, `to_double`, `to_num`, `to_char`, `to_byte`, and `to_short`; and fixed-width bitwise operations `bit_not`, `bit_and`, `bit_or`, `bit_xor`, `bit_and_not`, `bit_clear`, `bit_set`, `bit_flip`, `bit_test`, `bit_shift_left`, `bit_shift_right`, and `unsigned_bit_shift_right`. These operations MUST obey the applicable compile-time capability, representability, overflow, division-by-zero, shift-count, floating-point, and preflight requirements. Raw floating-point values MUST remain excluded from operations that require the stable equality, identity, or total-order capabilities defined by this document.

REQ-FN-002N. The supported free-function vocabulary MUST include `can_add`, `can_subtract`, and `can_multiply` for checked fixed-width arithmetic preflight. The corresponding arithmetic functions MUST use the documented checked default behavior; any wrapping or saturating alternatives MUST use distinct names.

## Stored Closures and Callable Convenience Family

REQ-VAL-021. Stored closures MUST be supported as collection elements and map values in the supported collection contract when their closure types satisfy the required bounded representation, construction, copying, moving, destruction, and non-allocation capabilities. Storing a closure MUST NOT grant it equality, ordering, map-key, set-element, or serialization capabilities.

REQ-FN-025. The first-pass callable convenience family MUST include `identity`, `constantly`, `complement`, `partial`, `fnil`, `juxt`, `every_pred`, `some_fn`, and `apply`, in addition to the `comp` behavior defined by this document. Each callable constructor or adapter MUST express compatible parameter and return types through concepts or equivalent compile-time constraints, MUST return a bounded callable representation when it returns a callable, and MUST preserve the invocation and non-allocation requirements of its captured functions and values. `juxt`, `every_pred`, and `some_fn` MUST define their short-circuit, result-shape, and invocation-order behavior; `partial` and `fnil` MUST define their captured-argument and default-argument behavior; and `apply` MUST define its supported final sequence argument and capacity behavior. These functions MUST remain ordinary callable operations and MUST NOT provide transducer semantics, runtime type dispatch, hidden mutable caching, or implicit callback retention by collection results.

## Traceability and Related Requirements

- **Downstream Artifact**: Numeric functions, checked arithmetic helpers, bitwise functions, and callable builder adapters (`partial`, `juxt`, `fnil`, `complement`, `apply`).
- **Governed REQs**: `REQ-NUM-000`–`017`, `REQ-FN-002N`, `REQ-VAL-021`, `REQ-FN-025`.
