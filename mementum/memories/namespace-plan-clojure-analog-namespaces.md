---
type: Decision
symbol: 🎯
title: namespace-plan-clojure-analog-namespaces
---
The library namespaces mirror Clojure's library organization:

- `cljonic::core` (and the `cljonic::` root): houses all core collection types (`Vector`, `Map`, `Set`, `Queue`, `String`, `MapEntry`) and core primitive operations (`conj`, `assoc`, `dissoc`, `disj`, `peek`, `pop`, `first`, `rest`, `count`, etc.).
- `cljonic::set`: will house relational and set-theoretic operations (equivalent to `clojure.set`, e.g. `union`, `intersection`, `difference`, `subset`, `superset`, `select`, `project`).
- `cljonic::string`: will house string-specific utilities and transformations (equivalent to `clojure.string`, e.g. `join`, `split`, `trim`, `upper-case`, `lower-case`, `replace`).

Concrete collection types `Set` and `String` live in `cljonic::` / `cljonic::core`, not in `cljonic::set` / `cljonic::string`. The latter namespaces are purely functional utility namespaces matching Clojure's namespace boundaries.
