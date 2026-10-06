---
type: Decision
symbol: 🎯
title: doxygen-examples-use-free-functions
---
Doxygen `\b Examples` blocks in `src/*.hpp` interact with cljonic values only
through value-level syntax and the free-function API — never `<instance>.<method>`
calls (`x.count()`, `x.contains(k)`, `v.index()`). Member `.method` access is
implementation detail, meant only for the bodies of the free functions, not
user-facing style — even when it compiles.

Allowed surface in an example: construction (CTAD, capacity/argument forms), the
instance-call `operator()` where the type provides it (e.g. `Vector`'s callable
lookup `values(0)` — required, and NOT a member call), documented interop
accessors (`view()`, `begin()`) where the API requires documenting them, and
free functions in a free-function header.

Placement split (kept): a TYPE's own example (`Vector`, `Range`, `Variant`,
`MapEntry`) shows only construction + its own callable/interop surface and calls
NO free functions; each free function's OWN header demonstrates that function
with a few representative use cases. Types show how to construct, functions show
how to use. Examples are illustrative, not exhaustive: a handful of cases that
convey the basics is enough; do not enumerate every type, overload, or arity.

When a type's free-function API is still deferred (e.g. `cljonic::Variant`,
Slice B), its type example stays construction-only until that API exists. A
particularly unfamiliar type (e.g. `cljonic::Variant`) is worth showing
explicitly in a relevant free function's example — `equal`/`not_equal` each
include one `Variant` operand — even though not every type must appear.
