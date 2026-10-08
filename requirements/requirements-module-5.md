# cljonic Requirements - Module 5: Higher-Order Algorithms & Traversal

## Purpose and Scope

This module records future higher-order sequence transformation requirements and specifies the collection-shaping/traversal inventory. The free-function APIs `map` (multi-source mapping) and `comp` are explicitly deferred; this does not defer the `cljonic::Map` collection type or its existing operations.

## Deferred Higher-Order Free Functions (Non-binding)

> **Deferred future work — non-binding.** The public free functions `map` (multi-source mapping) and `comp` are not part of the current supported API. Their requirements below record intended future behavior only; neither function is implementation-ready until its lifecycle is approved and its behavioral specification, tests, and implementation are propagated. This deferral does not apply to `cljonic::Map` or existing Map operations.

### Function Composition (`comp`) & Multi-Source `map`

λ REQ-FN-032(x).
  ∀ `map`: accepts its transforming function as the first argument, followed by one or more compatible source collections
  ∧ ∀ multi-source `map`: invokes the function with corresponding values from the source collections according to its documented termination and capacity policy
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-033(x).
  ∀ `comp`: accepts zero or more compatible element functions ∧ composes them into one callable value
  ∧ ∀ zero functions: produce an identity callable ∧ ∀ one function: produces an equivalent callable
  ∧ ∀ functions supplied as `comp(f, g, h, ...)`: compile-time checking verifies from right to left that each function accepts the return type of the function to its right ∧ the composed function has the rightmost function's input type and the leftmost function's return type
  ∧ ∀ resulting_callable: applies the functions from right to left, representing `f(g(h(value)))` for three functions
  ∧ ∀ incompatible parameter ∧ return types: rejected at compile time ∧ ∀ context: ¬∃ transducer composition ∨ destination-independent collection-processing semantics
  rationale: transducers and destination-independent processing sit outside the bounded, materialization-first model of the library
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

## Collection-Shaping and Traversal Family

λ REQ-SEQ-022(x).
  ∀ context: the first-pass collection-shaping and traversal family includes bounded operations corresponding to `take`, `drop`, `take_while`, `drop_while`, `take_last`, `drop_last`, `take_nth`, `nth`, `nthnext`, `nthrest`, `butlast`, `map_indexed`, `rseq`, `second`, `ffirst`, `fnext`, `nfirst`, `nnext`, `some`, `is_every`, `not_any`, `not_every`, `distinct`, `dedupe`, `frequencies`, `reductions`, `split_at`, `split_with`, `mapcat`, `interleave`, `interpose`, `partition`, `partition_all`, `partition_by`, `partitionv`, `partitionv_all`, `group_by`, `flatten`, `tree_seq`, `keep`, `keep_indexed`, `remove`, `replace`, `mapv`, `filterv`, `subvec`, `find`, `reduce_kv`, `sort`, and `sort_by`
  ∧ ∀ listed_operation: an individually named downstream behavioral specification exists before it is treated as implementation-ready
  ∧ ∀ specification: defines the operation's public arity and argument roles, callback contract where applicable, termination rule, equality and ordering capabilities, nested-result representation, producer behavior, result capacity derivation, typed-absence behavior, failure/preflight behavior, and complete-versus-bounded-result classification
  ∧ ∀ operation_level_contract: ¬∃ inference only from this inventory
  ∧ policies:
    1. ∀ operation: preserves input values ∧ ¬∃ introduction of side effects or hidden mutable state independently of pure callbacks
    2. ∀ operation ∈ {`distinct`, `dedupe`, `frequencies`, `sort`, `sort_by`} and similar: requires the applicable `stable_equality_comparable` or `totally_ordered` capability for their elements/keys
    3. ∀ operation: produces bounded owning results when a useful finite capacity can be derived, or explicit producers when results are unbounded
    4. ∀ traversal over maps ∧ sets: treats iteration order as semantically unordered
    5. ∀ (transducer-only arities ∧ hidden lazy-sequence machinery): unsupported
  {source: stakeholder_decided, decided_by: original_spec_author}

## Traceability and Related Requirements

- **Downstream Artifact**: Free-function templates for sequence shaping, slicing, filtering, mapping, grouping, and ordering.
- **Governed REQs**: `REQ-SEQ-022`, `REQ-FN-032`, `REQ-FN-033`.