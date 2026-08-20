---
type: Insight
symbol: 💡
title: lcov-path-anchoring-for-src-coverage
---
Use `$(CURDIR)/src/*` (not `*/src/*`) as the lcov `--extract` pattern for project source coverage. The glob `*/src/*` also matches `_deps/catch2-src/` and other fetched deps that contain `/src/` in their path, inflating or contaminating coverage scope. Anchoring to the absolute project root ensures only production headers in `src/` are measured.
