---
type: Decision
symbol: 🎯
title: header-guard-convention
---
All `src/**/*.hpp` and `tests/**/*.hpp` headers MUST use `#pragma once` as their sole include guard. `#ifndef CLJONIC_*_HPP` include guards are not used.

Decided 2026-09-29 (human) after finding `src/` split 23 include-guards vs 14 `#pragma once` with no documented convention and no gate enforcing either.

Rationale: one line, self-maintaining, and it removes the mismatched/missing-`#endif` bug class. It was already the style of the flagship public types (`Vector`/`Map`/`Set`/`Queue`/`String`), `cljonic-config.hpp`, and all four test headers. It is supported by every relevant toolchain (GCC/Clang and embedded GCC/Clang/IAR/Keil).

Considered and deliberately not chosen: C++ Core Guidelines **SF.8** prefers include guards for strict standards portability.

Enforced by `make header-guards` (`scripts/check-header-guards.py`), wired into `upsert-gate`, `validate`, and `git`. The generated single header `cljonic.hpp` is not scanned directly; it inherits `#pragma once` from the sources.
