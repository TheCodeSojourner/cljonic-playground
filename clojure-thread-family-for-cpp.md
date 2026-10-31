# Prompt: Specify the C++ Threading Family (`thru`) for cljonic

> **Phase gate.** This prompt authorizes **durable-artifact work only**: requirements, vocabulary, architecture, and Allium specifications. It does **not** authorize implementation, source changes, tests, examples, build registrations, or generated output. Those belong to a later, explicitly approved phase.
>
> **Provenance.** This is the operative prompt for the threading family. It semantically supersedes `clojure-thread-first-and-last-for-cpp.md`, whose design intent is absorbed below. That earlier file is retained only as history and must not be treated as the source of truth. The decisions in Section 3 were taken with the human in a session on 2026-10-30; cite that date and this file when recording requirement provenance.

## 1. Purpose and Outcome

Bring the expressiveness of Clojure's threading macros into cljonic as a small, compile-time-checked, value-oriented facility — and do it the way this repository works: **durable artifacts first, code later**.

The family is five named facilities plus one terminal: `thru`, `as_thru`, `cond_thru`, `some_thru`, `doto_thru`, and `thru_value`. The long-term outcome follows the core design brief: a pipeline that reads like Clojure's `->` and `->>`, with `>` selecting thread-first argument insertion, `>>` selecting thread-last insertion, variadic stage arguments, arbitrarily long chains, one consistent direction per pipeline enforced by the type system, and a terminal that yields the final value. The family facilities add conditional, absence-aware, and Atom-directed behavior on top of the same engine.

The **immediate** outcome of this prompt is narrower and is what you must deliver in this session:

1. A decision on whether the family belongs in cljonic at all (the domain restriction in Section 3.2 strengthens that case, but it is still a decision).
2. If it belongs: approved **requirements** clauses for each in-scope facility and for the cross-cutting pipeline invariants.
3. Corresponding **vocabulary** entries and lifecycle classifications.
4. **Architecture** entries that place the feature in the durability hierarchy and record any constraint conflicts.
5. **Allium specifications** for each approved behavior.
6. Nothing else.

## 2. Working Instructions for the AI

1. **Orient before proposing.** Read `mementum/state.md`, follow its related links, and recall relevant memories and knowledge pages. Inspect the current requirements, vocabulary, architecture, and specs. Do not assume this prompt's summary of those artifacts is accurate or current.
2. **Decide the boundary before designing.** Check `requirements/`, `architecture.md`, `vocabulary.md`, and `specs/` for governing contracts. Surface every conflict with bounded storage, purity, no-heap, no-exception, no-RTTI, value semantics, single-threaded, closed-vocabulary, or naming guarantees; do not silently weaken an existing requirement to make room.
3. **Follow the repository's process.** Use the matching GYBIS skills and gates. Preserve stable designators, provenance records, lifecycle classifications, dependency order, and cross-artifact traceability. Do not invent designators. Do not encode implementation detail as behavioral truth.
4. **Elicit before writing on anything behavioral.** Section 3.4 lists open decisions. Where a choice changes public behavior, type constraints, effect policy, or composability, ask a focused question. Distinguish human decisions from analysis-derived recommendations and mark the difference in the artifacts.
5. **Stay in this phase.** No implementation, no tests, no examples, no headers, no build changes, no generated artifacts. Report exact files and gates touched. Update `mementum/state.md` after each significant approved change.
6. **Do not assume the whole family is in scope.** The human may approve a subset, defer a facility, or reject the proposal. If scope is unclear, stop at a proposal and ask. A names-only decision (Section 3.1) is not a scope decision.

## 3. Decision Status

### 3.1 Settled — naming only (do not re-litigate)

The public names are decided: `thru`, `as_thru`, `cond_thru`, `some_thru`, `doto_thru`, and the terminal `thru_value`.

This is a **naming-only** decision. It fixes spelling, not semantics, not scope, and not inclusion. If a facility is approved for the library, it carries this name unchanged. If a facility is rejected or deferred, the name is not reused for anything else. Note the deliberate suffix asymmetry: `*_thru` names are stage factories; `thru_value` is the terminal. Preserve that distinction in documentation and diagnostics, and reject `x > thru_value(...)` with a targeted message ("`thru_value` terminates a pipeline; it cannot appear as a stage").

### 3.2 Settled — structure and scope (approved by the human)

**Uniform policy-tagged stages.** There is **one pipeline engine and one public pipeline surface**, not five separate pipelines. Every stage carries its facility in its compile-time type; `operator>` and `operator>>` are constrained on the stage concept and dispatch on stage kind. Stage facilities interleave freely in one pipeline. A stage's meaning follows its own kind, per the transition contract in Section 8 — it is not reinterpreted by pipeline context, except for the documented auto-lift rule inside a Maybe pipeline (Section 6.3), which is the faithful port of Clojure's `some->`.

**Value-oriented domain.** Stages invoke **cljonic free-function APIs**; the threaded values are **cljonic collections, cljonic producers, and C++ simple scalar-like entities** — scalars such as `int`, `long`, `float`, `double`, and `char`, and user-defined types that act like simple scalars, such as instances of a simple aggregate struct with public data members, no user-defined methods, no private data, and non-allocating value operations. Arbitrary external containers, standard-library views, and move-only types are **not** in the pipeline domain.

Consequences the design must absorb (do not re-litigate them):

- All admitted values are nothrow-copyable, so move-only support, consumed-rvalue adapters, and `std::unique_ptr`-style machinery are **out of scope**.
- Member function pointers and `std::invoke`-based dispatch are **out of scope**; free functions, lambdas, and function objects are invoked directly.
- Because cljonic public operations return owning values (`REQ-VAL-008`), references arise only from user-supplied lambdas that deliberately return one; the policy for that is settled below.
- The pipeline holds each intermediate value and **moves it forward** into the next invocation rather than copying; a `Vector<int, 1000>` must not be copied at every stage.
- When every stage is `constexpr`, `noexcept`, and non-allocating, the pipeline is unconditionally so; there is no conditional-`noexcept` design problem to solve, only an obligation to state it.
- cljonic free functions are templates, so `v > thru(count)` does not deduce. The design must name the supported spellings — most plausibly a lambda (`thru([](auto const& c) { return count(c); })`), making the `as_thru` idiom the normal case rather than a convenience. Specify this explicitly; it determines how the API reads in practice.

**`doto_thru` is narrowed to `Atom<T>`.** A `doto_thru` stage requires the current pipeline value to be a `cljonic::Atom<T>`; it applies the supplied effect-oriented cljonic operations (`swap`, `reset`) against that atom, discards their results, and leaves the atom as the current value. `>` only — there is no Clojure `doto->>`, and inventing one invents semantics.

Consequences:

- **No new purity exception is required.** Atom mutation is already the library's one sanctioned, explicit effect (`REQ-VAL-009`), so `doto_thru` inherits an approved effect scope instead of creating one.
- **The atom must be borrowed, not copied.** An `Atom` is a mutable reference-like cell; passing it by value would apply the effects to a discarded copy. Specify that a `doto_thru` pipeline requires an lvalue atom and documents the lifetime obligation.
- Without the `Atom` narrowing, `doto_thru` would have no purpose against immutable cljonic values; do not generalize it back.
- **`Atom<T>` is the only reference-like cljonic entity.** No other cljonic value requires borrowing. Do not generalize reference semantics to collections or producers; `doto_thru` is the single place where by-value transfer would be wrong.

**Callables and reference results.** Stages are cljonic free functions, lambdas, and function objects, invoked directly. A stage may return a reference **only** for an intermediate position, as a copy-avoidance optimization inside the pipeline; `thru_value` always yields a value, so no reference escapes. The supporting constraints are specified in Section 5.7: the terminal returns by value, a reference result pins its owner against move-forwarding, the intra-pipeline lifetime obligation is a documented caller responsibility, and `some_thru` stages must yield values because a `Maybe` cannot hold a reference.

### 3.3 Approved in principle — `cljonic::Maybe` (its own design cycle still required)

**`cljonic::Maybe<T>`.** Approved in principle by the human as the absence model; the value type's *design* remains open and must pass the vocabulary gate. The absence model for `some_thru` is a library-defined bounded value type — empty or holding exactly one `T` — replacing `std::optional`, which is not a cljonic value and is outside the closed vocabulary (`REQ-VOCAB-010`). This mirrors the established precedent of `cljonic::Variant` replacing `std::variant` (`REQ-CAP-011`).

Treat `Maybe` as a **library-wide value type requiring its own requirements, vocabulary, architecture, and specification cycle** — not a sub-bullet of the threading family. Candidate properties to elicit: bounded automatic storage; nothrow default/copy/move/destruction; no allocation, exceptions, or RTTI; `constexpr`; no valueless state; presence predicate and default-returning access consistent with `REQ-BOUNDS-*`; equality exactly when `T` admits stable equality and ordering exactly when `T` is totally ordered (a `Maybe` of a float-bearing struct is storable but not comparable, matching existing rules); `NothrowCollectionElement` compatibility so a `Maybe` can itself be stored in collections; and a minimal operation surface. The vocabulary name itself must pass the naming gate.

**Sequencing:** `Maybe` first (or in parallel as a separate feature), `some_thru` second. Do not specify `some_thru` against a not-yet-specified absence type.

### 3.4 Open (elicit and record; do not decide unilaterally)

- Whether the family belongs in cljonic at all, and which facilities are in scope (the domain restriction strengthens the case; it is still a decision).
- Whether `as_thru` is an alias factory returning the base stage type or a distinct stage type.
- The `cond_thru` predicate contract (does it observe the current value?), evaluation order, and side-effect policy.
- Whether `some_thru` is in the first approved slice at all, given the `Maybe` dependency.
- The `Maybe` design itself (Section 3.3).
- Minimum supported C++ standard beyond the project's existing baseline.
- Lifecycle classification of every facility and operation.

## 4. Clojure Semantics the Family Must Respect

These are the reference behaviors. C++ cannot reproduce them through syntax rewriting, so every facility needs an explicit, approved C++ contract rather than a claim of exact macro equivalence.

| Clojure | Insertion | Expansion or note |
|---|---|---|
| `->` | value becomes the **second item** (first argument) | `(-> x (f a b) g)` ≡ `(g (f x a b))` |
| `->>` | value becomes the **last argument** | `(->> x (f a b) g)` ≡ `(g (f a b x))` |
| `some->` / `some->>` | thread-first/last insertion with nil short-circuiting | Remaining forms are not evaluated after the first nil intermediate. |
| `cond->` / `cond->>` | thread-first/last insertion, conditional on paired truthy tests | `(cond-> x p (assoc :ready true))` keeps `x` if `p` is falsey; otherwise it applies `assoc`. |
| `as->` | binds a name; the expression may use it anywhere | `(as-> x $ (f a $ b))` places `x` in `f`'s second argument. |
| `doto` | invokes forms against the original for effect, returns the original | Conceptually binds the input once, calls `(f input a)` and `(g input)`, then returns that same input. |

The table gives one-line orientation only. Semantics that the C++ contracts must not get wrong:

- `cond->` and `cond->>` take alternating `test form` pairs. Tests are evaluated in order, do **not** receive the threaded value implicitly, and are not threaded into — a C++ predicate that observes the current value is a deliberate adaptation, not a port.
- `as->` does not thread positionally at all. The bound value may appear in any argument position, any number of times, or not at all.
- `some->` short-circuits on the *first* absence, including an absent initial value, and never evaluates the remaining forms.
- There is **no** Clojure `doto->>`.

Additional reference facts that constrain the C++ design:

- A non-list form is treated as a one-argument call: `(-> x f)` and `(->> x f)` both become `(f x)`.
- An empty chain returns its input: `(-> x)` ≡ `x`.
- Macro expansion is total and static. There is no runtime threading object in Clojure, and no first-class "pipeline value."
- Threading is positional rewriting of forms, not function composition. It offers no point-free semantics.

## 5. Core Facility: `thru`

The subsections below define the core contract to elicit and record, within the decided structure and scope of Section 3.2. None of it is binding until approved and written into the durable artifacts.

### 5.1 Surface

```cpp
auto result = thru_value(x
    > thru(function_a, arg1, arg2)
    > thru(function_b, arg3)
    > thru(function_c));

auto result = thru_value(input
    >> thru(function_a, arg1, arg2)
    >> thru(function_b, arg3)
    >> thru(function_c));
```

`thru` accepts a callable plus zero or more additional arguments, stored in the adapter and expanded at invocation. `thru_value(...)` is the terminal: it takes the final pipeline expression and yields the plain result, so `auto` deduces the actual result type and the internal wrapper never escapes.

### 5.2 Threading semantics

Thread-first: `x > thru(f, a, b)` invokes `f(x, a, b)`.
Thread-last: `x >> thru(f, a, b)` invokes `f(a, b, x)`.

The same `thru` factory supports both directions. Both operators must support zero, one, and many additional arguments, with no artificial arity limit.

### 5.3 Variadic arguments

Use variadic templates and standard mechanisms, not fixed-arity overloads. Additional arguments are stored in the adapter and expanded in their original relative order on either side of the threaded value.

### 5.4 Pipelines and mode consistency

A pipeline is arbitrarily long; each stage's result feeds the next. A pipeline uses exactly one direction. **Mixing `>` and `>>` within one pipeline is unsupported and must be rejected at compile time.**

Encode the direction in the pipeline's compile-time type: the first operator establishes the mode; every later stage must match; the other operator must fail to compile. Documentation and runtime checks are insufficient. Use constrained overloads, concepts, or equivalent standard mechanisms so the invalid combination is unavailable rather than merely discouraged. Do not attempt to rescue mixed pipelines by parenthesization or by leaning on precedence.

### 5.5 Operator precedence

Overloaded operators do not change C++ precedence or associativity. `>>` binds more tightly than `>`, and both are left-associative. For a single-direction chain this evaluates stages left to right as intended; confirm and explain this in the architecture or design notes. Mixed-direction chains must be forbidden precisely because precedence would otherwise silently reorder them. The terminal call also removes the fragile parenthesization that a member terminal would require (`x > thru(f) > thru(g).value()` binds `.value()` to the stage, not the pipeline).

### 5.6 Result extraction: `thru_value`

The internal pipeline wrapper must never be the observable final result. `thru_value(pipeline)` returns the actual **value** of the final stage and must:

- return the result **by value**, moved from the final accumulator when it is owned, so that no reference ever escapes the pipeline;
- **not** use `decltype(auto)` for its return type. Preserving a reference at this boundary is precisely the escape this rule prevents; an earlier draft that preserved reference results is superseded;
- preserve the final result's cv-qualification only to the extent a by-value return does under normal C++ semantics;
- be constrained on the pipeline type so that a non-pipeline argument is a targeted compile-time diagnostic.

Document the consequence: when the final stage returns a reference, exactly one materialization occurs at the terminal. That copy is the deliberate price of a boundary that cannot dangle.

`auto`, `const auto`, and `decltype(auto)` declarations of the `thru_value` result must behave by normal deduction rules from the by-value result. Verify with compile-time assertions (`decltype`, `std::same_as`, `std::is_same_v`) that the terminal's type is never a reference. Reference **intermediate** results are permitted (Section 5.7); reference **terminal** results are not.

Explain in the durable artifacts **why** the wrapper is necessary (compile-time mode enforcement and stage validation) and why `thru_value` is the explicit boundary between the internal representation and the caller's value. As a free function it also matches the library's preference for free functions over member APIs (`REQ-FN-001`).

### 5.7 Value categories and ownership (narrowed domain)

Within the decided value domain, simplify the original brief:

- Adapters are copyable and reusable; there is no consumed-rvalue or move-only adapter machinery.
- The pipeline holds each intermediate value and moves it forward into the next invocation; avoid copies of bounded collections at every stage.
- An lvalue pipeline exposes its current result as an lvalue to the next stage; a const lvalue pipeline respects constness; an rvalue pipeline forwards its current result as an rvalue. `thru_value` moves from an rvalue pipeline and copies from an lvalue pipeline, always yielding a value.
- Never silently turn an lvalue result into an rvalue or the reverse, and never retain a reference to a temporary past its lifetime.
- **Reference results are an intermediate-only optimization.** A stage may return `const T&` to avoid copying inside the pipeline. The productive case is projecting a field out of a simple aggregate struct, for example `thru([](auto const& s) -> const auto& { return s.field; })`.
- **Pinning rule.** When a stage returns a reference into the current accumulator, the pipeline must not move from that accumulator until a later stage replaces it with an owned result. Move-forwarding applies only when the previous stage produced a value; otherwise the optimization becomes a use-after-move.
- **Unchecked lifetime obligation.** C++ cannot distinguish a reference to the stage's input from a reference to a temporary inside the callable, so the contract is: a reference result must refer to the stage's input or to storage that outlives the pipeline. Record this as an explicit caller obligation — it is the one place where the library's static guarantees stop and caller discipline begins.
- **Per-facility exclusions.** A `some_thru` stage must yield a value, because a `Maybe` cannot hold a reference and no normalization rule exists for one. A `cond_thru` stage may pass an existing reference through unchanged. `doto_thru` is inherently reference-based because it borrows its atom.

### 5.8 Compile-time type safety (fundamental, not optional)

The compiler validates every stage before execution:

- each stage's actual output type must be compatible with the next stage's expected input;
- invalid pipelines fail to compile rather than failing at runtime;
- validation accounts for thread-first versus thread-last argument placement;
- each callable is checked for invocability with the threaded value and its stored arguments in the correct positions;
- normal overload resolution and implicit conversions are respected where appropriate;
- lvalue, const-lvalue, and rvalue categories are preserved when determining invocability;
- compatibility is validated across the **entire** pipeline, not stage by stage in isolation;
- the selected direction is consistent at every stage;
- every value flowing through the pipeline satisfies the domain of Section 3.2.

Prefer concepts and standard invocability facilities over errors that surface deep inside template instantiation. Provide useful diagnostics that identify the failing stage and the nature of the incompatibility.

### 5.9 Void-returning stages

A void-returning stage cannot supply a value to the next stage. Decide and document the policy. Unless a compelling reason exists, **reject void-returning intermediate stages at compile time**, and specify precisely what `thru_value` does if a final void stage is supported at all.

### 5.10 Implementation constraints (for the later phase; record as design intent now)

Prefer standard facilities: variadic templates, tuples, perfect forwarding, `std::forward`, ref-qualified members where still relevant, concepts, type traits, `decltype(auto)`. Avoid unnecessary complexity and external dependencies. Do not require C++26 reflection or other experimental features. Identify the minimum practical standard.

Because the value domain is closed and nothrow-copyable, no `std::invoke` dispatch, member-pointer handling, or move-only plumbing is required. Keep the engine small; that simplicity is a direct consequence of the domain decision and should be recorded as such.

## 6. Family Facilities

Each subsection states the Clojure reference and the contract to elicit, within the decided structure of Section 3.2. **Semantics remain proposals pending approval**; the names and the structural model are settled.

### 6.1 `as_thru`

Clojure's `as->` places the value anywhere. In C++ a lambda already does this: `thru([](auto&& v) { return f(a, v, b); })` is the direct equivalent, because the lambda chooses the position inside its body. Because cljonic free functions are templates, this lambda idiom is expected to be the **normal** spelling for cljonic stages, not a convenience.

Recommended: make `as_thru` an alias or adaptor that yields the **base stage type** of `thru`, so there is one stage vocabulary and no second pipeline universe. If the name adds no discoverability beyond documenting the lambda idiom, recommend omitting it — but the name itself is decided if it is shipped.

Open: whether it exists as an alias at all; how it is documented so nobody infers that C++ performs macro substitution or expression rewriting.

### 6.2 `cond_thru`

Mirror `cond->` and `cond->>`. Each stage pairs a predicate with a transformation; the transformation is applied only when the predicate is truthy. Specify: whether the predicate observes the current value, captures it, or ignores it; evaluation order; whether each predicate evaluates exactly once; whether predicates may have side effects; and how this interacts with the no-throw/no-allocation contract.

**The hard constraint:** a skipped stage must still leave a statically known value for the next stage, so every conditional stage is **endomorphic** (`T -> T`). This covers the common conditional collection update (`assoc`, `update`, `conj`, `dissoc`) and excludes conditional type-changing transformations. Treat it as an explicit constraint requiring human approval, with a targeted diagnostic naming the offending stage — not an incidental template failure. It is listed as open in Section 3.4 and the ledger; do not encode it as binding on the strength of this paragraph.

Open: predicate contract; whether `>` and `>>` both exist (Clojure has both); interaction with Maybe pipelines.

### 6.3 `some_thru`

Mirror `some->` and `some->>`, over `cljonic::Maybe` (Section 3.3). A stage result is normalized to an engaged-or-empty `Maybe`; an empty value stops the chain before later stages run; all stages must nevertheless be well-formed and checked at compile time.

Specify: how a bare initial value enters the lifted pipeline; the auto-lift rule — in a Maybe pipeline, a base `thru` stage receives the **unwrapped** value, matching Clojure's `some->` semantics exactly, and an absent state skips it; how plain `U` and `Maybe<U>` results are normalized (lift-and-flatten); whether nested `Maybe` values are flattened or rejected; and the terminal: `thru_value` yields the `Maybe<U>` result, with presence inspection and a defaulting access (name to be decided through the vocabulary gate).

This facility changes the value universe and introduces runtime short-circuiting; all of it depends on `Maybe` being specified first.

Open: whether it ships in the first slice; the `Maybe` design; nested-Maybe policy.

### 6.4 `doto_thru`

Narrowed to `Atom<T>` (Section 3.2). A `doto_thru` stage requires the current value to be an `Atom<T>`, applies each supplied effect-oriented cljonic operation against that atom, discards each result, and leaves the atom unchanged as the current value. The terminal yields the atom. `>` only.

Specify: that the atom is **borrowed** (an lvalue requirement with a documented lifetime obligation), that the admitted operations are the cljonic `Atom` operations, that each operation is invoked exactly once per stage application in order, and that the stage is the one effectful element of the family — justified by `Atom` mutation's existing sanctioned status rather than by a new purity exception.

Open: exact operation surface (`swap`, `reset`, both); whether a `doto_thru` pipeline may contain non-doto stages after an atom stage; diagnostic for a non-Atom current value.

## 7. Cross-Cutting Mechanisms

### 7.1 One engine, policy-tagged stages (decided)

There is one internal pipeline engine parameterized by direction, accumulated value type, and stage list. Each stage type carries its facility tag; the engine applies the transition contract of Section 8. The facilities are distinct public factory functions over shared stage and pipeline machinery — not separate pipeline types, and not one undifferentiated stage kind that hides policy changes. Document what the shared engine guarantees and why each facility is a distinct factory.

### 7.2 Operator discovery and ADL

The first expression of a pipeline is `value > stage` or `value >> stage`, where the left operand is an arbitrary admitted value. A hidden friend declared only on the *pipeline* type cannot be found there. Ensure the **initial** operator is discoverable through argument-dependent lookup — for example a constrained hidden friend on the stage type — while subsequent pipeline operators remain narrowly constrained on the pipeline type. Specify and test both discovery phases explicitly. Beware recursing back into the initial-operator overload when the left operand is already a pipeline.

### 7.3 Ambiguity with built-in `>` and `>>`

`>` and `>>` are existing comparison and shift operators. Keep adapters non-convertible to arithmetic types; constrain overloads to recognized stage and pipeline types; avoid broad catch-all overloads; prefer hidden friends where they help limit the candidate set. Document the trade-offs, including that `x > cond_thru(...)` with an arithmetic `x` still competes with built-in comparison and must be diagnosed clearly rather than producing a confusing ambiguity.

### 7.4 Storage and lifetime (narrowed domain)

- **Callable and argument storage.** Decayed-by-value default. Because all admitted values are nothrow-copyable, by-value storage is safe and `std::ref`/`std::cref` borrowing is rarely needed; the one exception is `doto_thru`'s atom, which must be borrowed by reference. Specify the atom's lifetime obligation explicitly.
- **Initial value storage.** Store by value (move from an rvalue initial value) and move forward between stages. Account for const lvalues and for pipelines that outlive their full expression.
- **Intermediate results.** No unnecessary copies; move-forwarding; no dangling references through internal storage; unavoidable lifetime restrictions documented and left with the caller.

Never silently retain a reference to a temporary past its lifetime.

### 7.5 Effects, exceptions, and `noexcept`

When every stage is `constexpr`, `noexcept`, and non-allocating, the pipeline is unconditionally so — state this as a guarantee, not a conditional. The single effectful exception is `doto_thru` on an `Atom`, whose effect scope is already sanctioned by the library's existing Atom contract. Do not extend that permission to any other facility or to arbitrary callbacks without a recorded decision.

### 7.6 Diagnostics

Name each public facility and each rejected rule in its diagnostics. Required messages at minimum: direction mismatch; a stage whose value is outside the Section 3.2 domain; a non-endomorphic `cond_thru` stage; a non-Atom current value for `doto_thru`; `thru_value` appearing as a stage; `thru_value` applied to a non-pipeline; an unsupported void intermediate stage; and, if `some_thru` ships, absence-model violations. Diagnostics must identify the failing stage and the violated constraint rather than exposing a template-instantiation trail. Assign lifecycle classification through the existing process.

## 8. Stage Transition Contract (decided structure; semantics to specify)

One pipeline; interleaved stage kinds; each stage's meaning fixed by its own kind. The accumulator transition table is the core specification artifact:

| Stage kind | Transition on current value `T` |
|---|---|
| `thru(f, args…)` / `as_thru(f)` | invoke `f` with `T` first or last → result type `U` (type may change); requires `U` in the Section 3.2 domain |
| `cond_thru(p, f)` | predicate evaluated; if falsey, `T` unchanged; if truthy, `f` applied endomorphically (`U` ≡ `T`) |
| `some_thru(f)` | if the accumulator's `Maybe` is empty, skip; else unwrap, apply `f` to the unwrapped value, lift-and-flatten the result into `Maybe<U>` |
| `doto_thru(op, args…)` | requires `T` ≡ `Atom<V>`; invoke the Atom operation against the borrowed atom; accumulator unchanged |

Rules that hold across the table:

1. **Direction consistency:** one direction per pipeline, enforced in the type (Section 5.4).
2. **Stage meaning is context-independent,** except the documented auto-lift rule inside a Maybe pipeline, which is the faithful port of `some->`.
3. **Domain closure:** every value at every point satisfies the Section 3.2 domain.
4. **No downgrade concept is needed** — there is one pipeline type; the stage list determines behavior.
5. **Reference results:** permitted for intermediate stages only; pinned against move-forwarding; rejected at the terminal, which always yields a value (Section 5.6).
6. **Diagnostics name the stage kind and the violated rule** for every rejected transition.

## 9. Testing Requirements (later phase — record now, execute on approval)

Comprehensive tests must cover, at minimum:

**Basic threading.** Thread-first and thread-last with zero, one, and multiple additional arguments, over collections, producers, scalars, and simple aggregate structs.

**Pipeline composition.** Consecutive same-direction stages; arbitrarily long pipelines; differing callable forms within the decided domain; differing intermediate types; left-to-right evaluation; terminal extraction via `thru_value`; rejection of a mismatched-direction stage in both directions; interleaved stage kinds from the transition table.

**Value categories.** Lvalue, const-lvalue, and rvalue initial values; lvalue, const-lvalue, and rvalue pipelines; move-forwarding (no per-stage copies of bounded collections); no dangling references.

**Callable support.** Cljonic free functions (with the decided template-disambiguation spelling), lambdas, and function objects.

**Compile-time type safety.** Valid pipelines compile; invalid pipelines fail — argument incompatibility in both insertion positions, incompatible intermediate types, constness and category incompatibility, invocability violations, direction mismatch, out-of-domain values, non-endomorphic conditional stages, non-Atom `doto_thru` values, `thru_value` misuse, unsupported void intermediates.

**Return types.** Compile-time assertions that `thru_value` never yields a reference; `thru_value` on lvalue and rvalue pipelines; correct behavior of `auto`, `const auto`, and `decltype(auto)` over the by-value result.

**Intermediate references.** Field projection without copying; pinning verified by absence of use-after-move; exactly one materialization at the terminal; rejection of a reference result in a `some_thru` stage.

**Family-specific.** Conditional skip and apply paths; conditional endomorphism rejection; short-circuit ordering and "later stages not invoked" for `some_thru`; `doto_thru` atom identity, borrow semantics, operation ordering, and effect visibility; `Maybe` round-tripping through lifted stages.

Use compile-fail tests to demonstrate rejection, and verify that each rejection is a *named* diagnostic rather than an arbitrary instantiation error.

## 10. Durable-Artifact Work Plan (this phase)

**Step 1 — scope and architecture fit.** Answer whether the family belongs in cljonic. Inspect governing architecture and existing requirement domains. Record conflicts (purity, no-heap, no-exception, no-RTTI, value semantics, single-threaded, closed-vocabulary, naming policy) and either resolve them through approved decisions or recommend deferral. Do not proceed to writing requirements while an unresolved conflict exists.

**Step 2 — elicit.** Walk Section 3.4 with the human. Record decisions with provenance and dates, marking which are human decisions and which are analysis-derived and approved. The Section 3.2 decisions are already human-approved; carry them forward verbatim.

**Step 3 — requirements.** Add clauses for each in-scope facility plus cross-cutting invariants: direction encoding and mixing rejection, stage transition contract, domain closure, value categories and move-forwarding, storage policy, terminal semantics, the Atom effect scope, diagnostics obligations, and lifecycle classification. Use the repository's designator conventions and stable identifiers. Do not renumber existing clauses.

**Step 4 — vocabulary.** Add the canonical terms introduced by the family (stage, pipeline, direction, stage kind, terminal, endomorphic stage, lifted pipeline, auto-lift, and so on), each with exactly one meaning used consistently in documentation, requirements, tests, and code.

**Step 5 — architecture.** Place the family in the durability hierarchy; record the constraint relationships (what the family may and may not do given the library's resource contracts); capture the precedence reasoning, the operator-discovery design, and the transition table; record unresolved drift explicitly rather than smoothing it.

**Step 6 — specifications.** For each approved facility, specify at least: public arities and argument roles; stage and predicate invocation contracts; operator and direction rules; interleaving behavior per the transition table; ownership and value categories; type-changing behavior; absence and short-circuit semantics; the Atom effect scope; terminal result type and its preservation guarantees; void behavior; failure modes and diagnostics; `constexpr`, `noexcept`, and allocation constraints; and compile-time versus runtime behavior. Give every behavior a traceable identifier.

**Step 7 — traceability and gates.** Maintain bidirectional requirement-to-specification coverage and run the applicable repository gates. Present all changes for human review at the project's approval gate.

**Step 8 — orient update.** Update `mementum/state.md` with what was decided, what was deferred, which artifacts changed, and which gates ran.

## 11. Deliverables

**Phase 1 (this session, on approval of scope):**

1. A written scope recommendation with architectural conflicts and their resolution.
2. Approved requirements clauses and vocabulary entries.
3. Approved architecture entries.
4. Allium specifications for each approved behavior.
5. A traceability report and a list of gates run.
6. An explicit list of deferred questions and unapproved proposals.

**Phase 2 (later session, only on explicit authorization):**

1. A proposed API summary and semantics statement.
2. An explanation of operator precedence implications.
3. An explanation of the internal pipeline representation and why `thru_value` is required.
4. A complete, compilable implementation.
5. Design decisions and trade-offs.
6. An explanation of compile-time pipeline checking.
7. Comprehensive unit tests and positive/negative compile-time tests.
8. Limitations and pitfalls.
9. Minimum required C++ standard and build instructions.

Deliver complete source code rather than pseudocode, in separate implementation and test files where appropriate.

## 12. Design Philosophy (retained)

The objective is a small, idiomatic, **value-oriented** C++ facility that brings Clojure's threading expressiveness into cljonic while combining: one `thru` factory; `>` and `>>` for argument insertion; variadic stages; natural fluent composition; one consistent direction per pipeline; compile-time enforcement of direction; safe, predictable storage over nothrow-copyable values; interleaved stage kinds under one transition contract; and comprehensive compile-time type safety across stages — all within the library's bounded, no-heap, no-exception, value-semantic, single-threaded contracts.

**Compile-time type safety is a core requirement**, not an enhancement: every stage is validated against the actual output of its predecessor, and incompatible pipelines are rejected during compilation.

Do not sacrifice API simplicity for unnecessary generality. Avoid abstractions that do not materially improve usability, safety, or correctness. Explain compromises explicitly and justify them in idiomatic C++ terms. Where requirements cannot be satisfied simultaneously, identify the conflict and recommend the simplest internally consistent solution **before** proceeding.

## Appendix: Decision Ledger (as of this prompt)

| Decision | Status |
|---|---|
| Facility names `thru`, `as_thru`, `cond_thru`, `some_thru`, `doto_thru` | Settled (naming only) |
| Terminal name `thru_value`; `*_thru` = stage factories, `thru_value` = terminal | Settled (naming only) |
| Uniform policy-tagged stages; one engine; interleaving allowed | **Approved** |
| Value domain: cljonic free functions, collections, producers, simple scalar-like types | **Approved** |
| Move-only, member-pointer, and consumed-adapter machinery removed | **Approved** (consequence) |
| Move-forwarding between stages; unconditional `constexpr`/`noexcept` when stages are | **Approved** (consequence) |
| `doto_thru` narrowed to `Atom<T>`; atom borrowed, not copied; `>` only | **Approved** |
| No new purity exception for `doto_thru` (Atom scope already sanctioned) | **Approved** (consequence) |
| `cljonic::Maybe` replaces `std::optional` for `some_thru` | **Approved in principle**; its own design cycle still required |
| Stages: cljonic free functions, lambdas, and function objects | **Approved** |
| Reference results: intermediate positions only; `thru_value` yields a value | **Approved** |
| `Atom<T>` is the only reference-like cljonic entity | **Approved** |
| `cond_thru` predicate contract; endomorphism approval | **Open** |
| `doto_thru` operation surface and post-atom stages; nested-`Maybe` policy; `cond_thru` both-operator support | **Open** |
| `as_thru` alias vs distinct; `some_thru` first-slice inclusion; minimum C++ standard | **Open** |
