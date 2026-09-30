# cljonic Requirements - Module 6: Numeric & Callable Convenience

## Purpose and Scope

This module specifies the checked fixed-width numeric contract, scalar comparison/selection, bitwise math, conversion/parsing restrictions, raw floating-point policies, closure storage, and callable convenience adapters (`partial`, `juxt`, `fnil`, `apply`, etc.). Module 6 ensures numeric and functional computations remain deterministic, non-allocating, and compile-time checked.

## Fixed-Width Numeric Contract & Policy

λ REQ-NUM-000(x).
  ∀ raw floating-point calculation ∧ comparison: treated as potentially non-deterministic across values, compiler settings, evaluation modes, and platforms because of rounding, representation, NaN, infinity, signed zero, and other IEEE-754 effects
  ∧ ∀ raw floating-point behavior: ¬∃ use for a stable semantic contract unless an explicit numeric policy defines and tests that behavior
  rationale: rounding, representation, NaN, infinity, and signed-zero effects make raw floating-point behavior non-deterministic across platforms
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-NUM-001(x).
  ∃ permitted_path: raw floating-point types stored as scalar values, vector elements, or map values when the selected operation does not require semantic equality or ordering, because those storage operations do not claim a stable floating-point result or identity contract
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-NUM-002(x).
  ∀ raw floating-point type: ¬∃ acceptance as map key ∨ set element because rounding, representation, NaN, infinity, and signed-zero behavior can make equality unsuitable as a stable identity across supported environments
  rationale: floating-point equality is unsuitable as a stable identity across supported environments
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-NUM-003(x).
  ∀ `range`: requires an integral or otherwise explicitly discrete numeric type
  ∧ ∀ raw floating-point bound ∨ step: rejected at compile time because rounding and evaluation differences can change termination and produced values
  rationale: rounding and evaluation differences can change termination and produced values, so range termination cannot be a stable contract over FP
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-NUM-004(x).
  ∀ operation requiring equality ∨ ordering: expresses that requirement through a named capability or equivalent compile-time constraint
  ∧ ∀ raw floating-point type: ¬∃ satisfaction of the default stable-equality or total-order capabilities because their results and comparisons may vary across supported environments
  rationale: floating-point results and comparisons may vary across supported environments, so they cannot back default equality/order capabilities
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-NUM-005(x).
  ∀ (sorting ∨ other ordered traversal) of raw floating-point values: fails at compile time because NaN, signed zero, rounding, and platform-dependent evaluation can prevent a stable total order
  ∧ ∃ permitted_path: a future explicitly configured total-order numeric wrapper provides the required capability ∧ ∀ raw `float` ∧ `double`: remain unsupported by the default policy
  rationale: NaN, signed zero, rounding, and platform-dependent evaluation prevent a stable total order for raw FP
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-NUM-006(x).
  ∀ (equality ∨ distinctness ∨ frequency counting ∨ other identity operation) over raw floating-point values: rejected unless an explicitly approved numeric policy provides the required semantics, because rounding and special-value behavior can make those results non-deterministic
  ∧ ∀ context: ¬∃ implicit epsilon comparison ∨ NaN normalization policy provided
  rationale: rounding and special-value behavior make FP identity results non-deterministic without an explicit numeric policy
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-NUM-007(x).
  ∀ collection operation that compares values: applies the same numeric capability constraints recursively to its element, key, and value types
  ∧ ∀ equality of collections containing raw floating-point values: rejected by default even when the collection itself can store those values, because nested floating-point comparisons may not be stable across supported environments
  rationale: nested floating-point comparisons may not be stable across supported environments, even when storage is permitted
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-NUM-008(x).
  ∀ numeric_conversion_operation: has an explicit target type ∧ ¬∃ selection of its result type through a global configuration constant
  ∧ ∀ cljonic equivalent of Clojure `int` when the conversion succeeds: returns a fixed-width signed 32-bit integer type
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-NUM-009(x).
  ∀ default numeric conversion with a statically known conversion that loses range, precision, or accuracy: rejects at compile time
  ∧ ∃ permitted_path: a statically known conversion that is provably exact and representable accepted in a constant expression
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-NUM-010(x).
  ∀ conversion whose safety cannot be determined at compile time: the library provides an explicitly checked, non-throwing runtime conversion whose result communicates success or failure without dynamic allocation or global mutable error state
  ∧ ∀ checked_result_type ∧ failure_states: documented before implementation
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-NUM-011(x).
  ∀ potentially lossy conversion from a runtime floating-point value to an integer: ¬∃ performance by an implicitly narrowing overload because rounding and representation differences can change the converted result ∧ requires an explicitly checked or explicitly lossy conversion operation named to make the policy visible at the call site
  rationale: the policy must be visible at the call site because rounding and representation differences can change the converted result
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-NUM-012(x).
  ∃ permitted_path: explicitly lossy numeric conversions provided as separate operations ∧ ∀ such_operation: its truncation, saturation, wrapping, NaN, infinity, out-of-range, and signedness behavior documented ∧ tested ∧ ¬∃ default conversion behavior
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-NUM-013(x).
  ∀ (numeric conversion ∧ text parsing): separate operations
  ∧ ∀ cljonic equivalent of Clojure `int`: accepts numeric or character inputs according to its documented source capabilities ∧ ¬∃ implicit parsing of a `String` ∧ string-to-integer conversion uses a separately named parsing operation with deterministic failure behavior
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-NUM-014(x).
  ∀ `parse_float`: documents its accepted syntax, sign, decimal, exponent, infinity, NaN, range, rounding, and accuracy behavior
  ∧ ∀ parser: ¬∃ making of raw floating-point parsing suitable for stable equality, ordering, or identity contracts without the explicit numeric policy required elsewhere
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-NUM-015(x).
  ∀ fixed_width_arithmetic: checked behavior by default
  ∧ ∀ runtime arithmetic operation: provides corresponding preflight predicates such as `can_add`, `can_subtract`, and `can_multiply` ∧ when the predicate is false, the operation returns its documented default ∧ leaves its inputs unchanged
  ∧ ∀ compile_time_known arithmetic overflow: rejected during constant evaluation
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-NUM-016(x).
  ∃ permitted_path: wrapping and saturating arithmetic provided ∧ ∀ such_operation: separately named ∧ ¬∃ implicit behavior of the default arithmetic operations
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-NUM-017(x).
  ∀ context: the first-pass numeric convenience family includes checked fixed-width arithmetic corresponding to `add`, `sub`, `mult`, `div`, `quot`, `rem`, and `mod`, with `can_add`, `can_subtract`, and `can_multiply` preflights; scalar comparison and selection corresponding to `compare`, `min`, `max`, `min_key`, and `max_key`; integer-step operations corresponding to `inc`, `dec`, and `negate`; constrained numeric predicates corresponding to `is_zero`, `is_positive`, `is_negative`, `is_even`, and `is_odd`; explicit conversions corresponding to `to_int`, `to_long`, `to_float`, `to_double`, `to_num`, `to_char`, `to_byte`, and `to_short`; and fixed-width bitwise operations `bit_not`, `bit_and`, `bit_or`, `bit_xor`, `bit_and_not`, `bit_clear`, `bit_set`, `bit_flip`, `bit_test`, `bit_shift_left`, `bit_shift_right`, and `unsigned_bit_shift_right`
  ∧ ∀ such_operation: obeys the applicable compile-time capability, representability, overflow, division-by-zero, shift-count, floating-point, and preflight requirements
  ∧ ∀ raw floating-point value: remains excluded from operations that require the stable equality, identity, or total-order capabilities defined by this document
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002N(x).
  ∀ context: the supported free-function vocabulary includes `can_add`, `can_subtract`, and `can_multiply` for checked fixed-width arithmetic preflight
  ∧ ∀ corresponding arithmetic function: uses the documented checked default behavior ∧ ∀ wrapping ∨ saturating alternative: uses distinct names
  {source: stakeholder_decided, decided_by: original_spec_author}

## Stored Closures and Callable Convenience Family

λ REQ-VAL-021(x).
  ∀ stored closure whose closure type satisfies the required bounded representation, construction, copying, moving, destruction, and non-allocation capabilities: supported as a collection element and map value in the supported collection contract
  ∧ ∀ storing of a closure: ¬∃ granting of equality, ordering, map-key, set-element, or serialization capabilities
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-025(x).
  ∀ context: the first-pass callable convenience family includes `identity`, `constantly`, `complement`, `partial`, `fnil`, `juxt`, `every_pred`, `some_fn`, and `apply`, in addition to the `comp` behavior defined by this document
  ∧ ∀ callable constructor ∨ adapter: expresses compatible parameter and return types through concepts or equivalent compile-time constraints ∧ returns a bounded callable representation when it returns a callable ∧ preserves the invocation and non-allocation requirements of its captured functions and values
  ∧ ∀ (`juxt` ∧ `every_pred` ∧ `some_fn`): define their short-circuit, result-shape, and invocation-order behavior ∧ ∀ (`partial` ∧ `fnil`): define their captured-argument and default-argument behavior ∧ ∀ `apply`: defines its supported final sequence argument and capacity behavior
  ∧ ∀ such_function: remains an ordinary callable operation ∧ ¬∃ (transducer semantics ∨ runtime type dispatch ∨ hidden mutable caching ∨ implicit callback retention by collection results)
  {source: stakeholder_decided, decided_by: original_spec_author}

## Traceability and Related Requirements

- **Downstream Artifact**: Numeric functions, checked arithmetic helpers, bitwise functions, and callable builder adapters (`partial`, `juxt`, `fnil`, `complement`, `apply`).
- **Governed REQs**: `REQ-NUM-000`–`017`, `REQ-FN-002N`, `REQ-VAL-021`, `REQ-FN-025`.