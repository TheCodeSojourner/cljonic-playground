# cljonic Requirements - Module 5: Higher-Order Algorithms & Traversal

## Purpose and Scope

This module specifies higher-order sequence transformation algorithms, function composition (`comp`), multi-source mapping, and collection shaping/traversal operations over any `cljonic_source`. Module 5 provides functional transformations while preserving value immutability, callback purity, and static bounds.

## Function Composition (`comp`) & Multi-Source Operations

REQ-FN-002G. `map` MUST accept its transforming function as the first argument, followed by one or more compatible source collections. Multi-source `map` MUST invoke the function with corresponding values from the source collections according to its documented termination and capacity policy.

REQ-FN-002H. `comp` MUST accept zero or more compatible element functions and MUST compose them into one callable value. Zero functions MUST produce an identity callable, and one function MUST produce an equivalent callable. For functions supplied as `comp(f, g, h, ...)`, compile-time checking MUST verify from right to left that each function accepts the return type of the function to its right; the composed function MUST have the rightmost function's input type and the leftmost function's return type. The resulting callable MUST apply the functions from right to left, representing `f(g(h(value)))` for three functions. `comp` MUST reject incompatible parameter and return types at compile time and MUST NOT provide transducer composition or destination-independent collection-processing semantics.

## Collection-Shaping and Traversal Family

REQ-SEQ-022. The first-pass collection-shaping and traversal family MUST include bounded operations corresponding to `take`, `drop`, `take_while`, `drop_while`, `take_last`, `drop_last`, `take_nth`, `nth`, `nthnext`, `nthrest`, `butlast`, `map_indexed`, `rseq`, `second`, `ffirst`, `fnext`, `nfirst`, `nnext`, `some`, `is_every`, `not_any`, `not_every`, `distinct`, `dedupe`, `frequencies`, `reductions`, `split_at`, `split_with`, `mapcat`, `interleave`, `interpose`, `partition`, `partition_all`, `partition_by`, `partitionv`, `partitionv_all`, `group_by`, `flatten`, `tree_seq`, `keep`, `keep_indexed`, `remove`, `replace`, `mapv`, `filterv`, `subvec`, `find`, `reduce_kv`, `sort`, and `sort_by`.

Each listed operation MUST have an individually named downstream behavioral specification before it is treated as implementation-ready. That specification MUST define the operation's public arity and argument roles, callback contract where applicable, termination rule, equality and ordering capabilities, nested-result representation, producer behavior, result capacity derivation, typed-absence behavior, failure/preflight behavior, and complete-versus-bounded-result classification. These operation-level contracts MUST NOT be inferred only from this inventory:

1. **Input Preservation & Purity**: Operations MUST preserve input values and MUST NOT introduce side effects or hidden mutable state independently of pure callbacks.
2. **Equality & Ordering Gates**: Operations such as `distinct`, `dedupe`, `frequencies`, `sort`, and `sort_by` MUST require the applicable `stable_equality_comparable` or `totally_ordered` capability for their elements/keys.
3. **Capacity & Result Representation**: Operations MUST produce bounded owning results when a useful finite capacity can be derived, or explicit producers when results are unbounded.
4. **Unordered Map/Set Semantics**: Traversal over maps and sets MUST treat iteration order as semantically unordered.
5. **No Transducers or Lazy Sequences**: Transducer-only arities and hidden lazy-sequence machinery remain unsupported.

## Traceability and Related Requirements

- **Downstream Artifact**: Free-function templates for sequence shaping, slicing, filtering, mapping, grouping, and ordering.
- **Governed REQs**: `REQ-SEQ-022`, `REQ-FN-002G`, `REQ-FN-002H`.
