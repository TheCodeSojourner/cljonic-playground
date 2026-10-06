---
type: Insight
symbol: 💡
title: coverage-measures-during-ai-upsert-loop
---
Coverage measures for the AI upsert loop are line-coverage based and enforced through `make coverage-cli COVERAGE_FILE=<header>`. The loop contract is: run after each code/test change and continue until stdout reports `coverage:lines=100.0%`. `coverage-cli` suppresses cmake/lcov noise, shows ctest output only on failure, and fails the command if coverage drops below `COVERAGE_THRESHOLD` (default 100). Scope control: `COVERAGE_FILE=foo.hpp` narrows measurement to `$(CURDIR)/src/foo.hpp`; otherwise the scope is project headers under `$(CURDIR)/src/*`. Path anchoring matters to avoid counting dependency sources (for example Catch2 `_deps/*/src`).

Agent/non-interactive runs use `make coverage-cli`: it is the same measurement, but it prints the percentage to stdout and opens no browser. `make coverage` is the **interactive** variant — it additionally runs `genhtml` to produce `build-coverage/coverage-report/` and launches `$BROWSER`. Never use `make coverage` from an agent/script context; it pops a browser tab. (The aggregate gates `git`/`validate`/`upsert-gate` already call `coverage-cli`.)