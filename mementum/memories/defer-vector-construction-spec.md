---
type: Pattern
symbol: 🔁
title: defer-vector-construction-spec
---
This memory records an earlier deferral of `VectorConstruction` while the minimal `count(Vector)` slice was being restarted. That deferral is superseded for Vector's implemented span path: construction now copies compatible `std::span` sources into owned bounded storage with constexpr, non-allocating, non-throwing behavior. Static-extent sources larger than capacity are rejected at compile time; dynamic-extent sources materialize a bounded prefix. Complete-fit validation belongs to the preflight/materialization pair: `fits_into` reports whether the whole source fits and `into` applies bounded-prefix materialization. A checked constructor is not planned because constructors remain total, bounded, and no-throw. The broader cross-collection source contract remains to be specified with tests and traceability.