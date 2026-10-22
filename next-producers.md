# Resume discussion: `next` for cljonic producers

Resume the postponed design conversation below. Initialize Gybis context with
`/gybis-init`, then read this handoff and verify the relevant current contracts
and implementation before proceeding. This document captures discussion and
recommendations, not approved behavioral requirements.

## Goal

Explore extending the public `next` free function to accept cljonic producers:
Range, Repeat, Iterate, Cycle, and Repeatedly.

No producer-support implementation or contract changes were made during the
discussion. Do not interpret this handoff as authorization to implement the
extension.

## Current baseline

- Before resuming producer-tail work, the human requested a separate correction
  to Repeat's counted argument order. The factory and constructor now use
  `repeat(count, value)` and `Repeat{count, value}`, matching Clojure. The
  examples below reflect that correction. Counts for Repeat and Repeatedly now
  admit signed as well as unsigned integer types whose nonnegative range fits
  `std::size_t`, excluding Boolean and character types. Nonpositive counts
  normalize to zero before unsigned conversion; nonpositive Repeatedly counts
  invoke no callbacks. The one-argument forms are unchanged.
- [next](src/cljonic-next.hpp) accepts Vector, Map, Set, Queue, and String.
  It returns an owning `Vector<C::value_type, C::capacity()>` containing the
  logical elements after the first, preserving the input. Empty and singleton
  inputs produce an empty Vector.
- [Next specification](specs/primitives/next.allium) currently describes the
  collection-only contract, including Vector results and matching capacity.
- Producers do not expose the collection-style static capacity required by the
  current implementation.
- [REQ-SEQ-021](requirements/requirements-module-4.md) prefers owning bounded
  results where a useful capacity can be derived, and explicit producer values
  where results are unbounded or lack a useful non-arbitrary static bound.
- The deferred [seq header](src/cljonic-seq.hpp) describes collection sequences
  as Vectors, but its placeholder implementation returns its input type. It is
  not an established universal sequence abstraction.

## Meaning of a sequence

Clojure's `next` returns a seq of the items after the first, or `nil` if none
remain. A seq is a sequential access abstraction, not necessarily a vector or
materialized storage.

Our discussion distinguished:

- "cljonic currently represents collection tails as Vectors" -- accurate.
- "all cljonic sequence results must be Vectors" -- not an approved universal
  rule, and problematic for unbounded producers.

The recommendation was to preserve existing collection behavior and return an
owning producer for producer inputs. Explicit `into` would materialize that
producer into a caller-bounded Vector. Empty producer values rather than
Clojure's `nil` were proposed, not approved.

## Preferred implementation direction from the discussion

The assistant initially suggested one owning tail-producer adapter. The human
then relayed a coworker's alternative: each producer implements a `tail()` or
`next()` member, and the public free function delegates to it.

The assistant revised its recommendation to favor producer-owned
implementation, with `producer.next()` as a reasonable member spelling:

```cpp
// Conceptual sketch only; not an approved constraint or signature.
template <SupportedProducer P>
constexpr auto next(const P& producer) noexcept {
    return producer.next();
}
```

Keep the free function as the primary documented API. Member naming and
visibility remain subject to approval. Do not require every producer member to
return its own type: an appropriate owning producer result may be needed.
A shared adapter remains an alternative, not a selected implementation.

Each producer implements a common contract rather than independently defining
incompatible meanings:

- Omit exactly the first logical element and preserve remaining order.
- Preserve the original producer value.
- Return a self-contained value independent of the original argument's lifetime.
- Preserve logical unboundedness when the tail remains unbounded.
- Avoid allocation, exceptions, and implicit materialization.

Existing producer iterators can contain pointers into their owning producer.
Do not return an adapter retaining iterators into a temporary or original
argument. Copy/move and temporary-source lifetime behavior require tests.

## Producer-specific semantics discussed

- **Repeat:** retain the repeated value. A finite count becomes
  `max(count - 1, 0)`; an unbounded repeat remains unbounded.
- **Range:** advance the start once while preserving endpoint and step, handling
  empty inputs, endpoint crossing, zero step, and arithmetic safety.
- **Iterate:** begin at the next generated value and continue the same step.
  Callback evaluation timing needs a decision.
- **Cycle:** skip the first element once, preserving the original cycle.
  The tail of cycling `[10, 20, 30]` is `20, 30, 10, 20, ...`, not a cycle of
  `[20, 30]`. Position representation remains undecided. Empty sources and
  unbounded producer sources need explicit treatment.
- **Repeatedly:** omit the first generated result, then continue generation.
  Counted forms have one fewer result. Callbacks may be stateful or have
  external side effects under existing requirements, so evaluation timing is
  observable and requires explicit approval.

## Unresolved design decisions

1. **Logical tail versus tail of a bounded observation.** With observation cap
   `M`, simply advancing an existing capped iterator exposes at most `M - 1`
   elements. Advancing the logical unbounded producer first and then applying
   its normal observation cap exposes `M`. The assistant favored the latter:
   repeated `next` calls should not exhaust a logically unbounded producer
   merely because of observation caps. This is not yet approved.
   Oversized finite Range values also require distinguishing true remaining
   extent from an already saturated count.
2. **Callback timing.** The assistant initially proposed that constructing a
   tail invokes no callbacks and traversal performs the skip. The later
   producer-member proposal left this open: directly advancing Iterate's seed
   invokes its step, and advancing Repeatedly can have external side effects.
   Do not treat deferred evaluation as an established decision.
3. **Empty and singleton Repeatedly behavior.** Under the deferred-skip proposal,
   a zero-count source invokes no callbacks; traversing a singleton tail could
   generate and discard its sole source value despite returning no elements.
   This was illustrative, not approved, and must be reconciled with existing
   callback contracts.
4. **Representation and integration.** Decide member spelling, any new result
   types, and nominal/capability registration. Producer results should work
   coherently with `count`, `is_empty`, `into`, and `fits_into`, without promising
   memoization or repeatable callback results.

## Exact stopping point

The human asked which producer to start with. The assistant recommended
**Repeat, starting with its finite counted form**, then its unbounded form.

Repeat is the smallest slice: no callback timing, generated-value arithmetic,
or cycle position. It can return the same `Repeat<T>` type.

Proposed cases:

| Input | Proposed tail |
|---|---|
| `repeat(3, 7)` | Finite Repeat of two sevens |
| `repeat(1, 7)` | Empty finite Repeat |
| `repeat(0, 7)` | Empty finite Repeat |
| `repeat(7)` | Unbounded Repeat of seven |

Verify input preservation, finite/unbounded identity, safe decrement at zero,
unchanged unbounded observation policy, temporary-source lifetime, and
compatibility with source-taking functions. Cover constant evaluation,
runtime execution, diagnostics, traceability, and no-heap evidence.

Range was suggested next, with Iterate, Cycle, and Repeatedly following after
their additional decisions.

## How to continue

Start by briefly confirming this stopping point. The next proposed action is
to formulate **Repeat-specific requirements for approval**, not edit code.
Do not reopen every alternative unless current evidence requires it.

Follow the project's approval-gated order: requirements, vocabulary,
architecture, behavioral specifications, tests, implementation, and directly
related documentation/integration. Pause for human verification at each
required layer. Do not change the existing collection result policy, approve
unresolved semantics on the human's behalf, stage files, or commit product
changes without authorization.
