#!/usr/bin/env python3
"""Diagnostic-coverage gate for REQ-DIAG-009 / REQ-DIAG-010.

Every public free function whose supported domain is closed must either carry a
targeted rejection diagnostic (a diagnostic overload built on
``dependent_false``) or record a documented exclusion. The obligation is
universal and forward-binding: a new header under ``src/`` is a conformance
defect until it is classified here.

Statuses
--------
- ``fallback``: the header must contain the diagnostic-fallback mechanism
  (``dependent_false``).
- ``pending``: subject to the requirement but not yet taught. Reported, not
  fatal, so the governance gate can land ahead of the application slices that
  add the fallbacks. This set is the application backlog and is expected to
  shrink to empty.
- ``excluded``: a documented exclusion (no fallback can be formed). The reason
  is recorded here; a matching note belongs in the requirement and the header.
- ``non_subject``: infrastructure or type-only header that defines no public
  free function subject to REQ-DIAG-009.

The gate fails if a ``src/`` header is unclassified, or if a header claimed as
``fallback`` lacks the mechanism.
"""

import pathlib
import sys

FALLBACK_MARKER = "dependent_false"

# header -> status
REGISTRY: dict[str, str] = {
    # Free-function and source-constructor fallbacks already in place.
    "cljonic-equal.hpp": "fallback",
    "cljonic-not-equal.hpp": "fallback",
    "cljonic-assoc.hpp": "fallback",
    "cljonic-can-assoc.hpp": "fallback",
    "cljonic-repeat.hpp": "fallback",
    "cljonic-cycle.hpp": "fallback",
    "cljonic-iterate.hpp": "fallback",
    "cljonic-repeatedly.hpp": "fallback",
    "cljonic-map.hpp": "fallback",
    "cljonic-queue.hpp": "fallback",
    "cljonic-set.hpp": "fallback",
    "cljonic-string.hpp": "fallback",
    "cljonic-vector.hpp": "fallback",
    # Subject to REQ-DIAG-009; fallback not yet added (application backlog).
    "cljonic-can-conj.hpp": "pending",
    "cljonic-conj.hpp": "pending",
    "cljonic-contains.hpp": "pending",
    "cljonic-count.hpp": "pending",
    "cljonic-disj.hpp": "pending",
    "cljonic-dissoc.hpp": "pending",
    "cljonic-fits-into.hpp": "pending",
    "cljonic-get.hpp": "pending",
    "cljonic-into.hpp": "pending",
    "cljonic-is-empty.hpp": "pending",
    "cljonic-peek.hpp": "pending",
    "cljonic-pop.hpp": "pending",
    # Documented exclusions.
    "cljonic-range.hpp": "excluded",       # element domain is a class-template constraint (signed_integral)
    "cljonic-map-entry.hpp": "excluded",   # aggregate-like; domain is type-argument gated
    "cljonic-variant.hpp": "excluded",     # free-function API deferred (Slice B)
    "cljonic-empty.hpp": "excluded",       # deferred non-public API (DEFERRED_NON_PUBLIC_HEADERS)
    "cljonic-first.hpp": "excluded",       # deferred non-public API (DEFERRED_NON_PUBLIC_HEADERS)
    "cljonic-next.hpp": "excluded",        # deferred non-public API (DEFERRED_NON_PUBLIC_HEADERS)
    "cljonic-not-empty.hpp": "excluded",   # deferred non-public API (DEFERRED_NON_PUBLIC_HEADERS)
    "cljonic-rest.hpp": "excluded",        # deferred non-public API (DEFERRED_NON_PUBLIC_HEADERS)
    "cljonic-seq.hpp": "excluded",         # deferred non-public API (DEFERRED_NON_PUBLIC_HEADERS)
    # Infrastructure / type-only headers: no public free-function subject.
    "cljonic-concepts.hpp": "non_subject",
    "cljonic-config.hpp": "non_subject",
    "cljonic-core.hpp": "non_subject",
    "cljonic-core-collection-maximum-element-count.hpp": "non_subject",
}

VALID_STATUSES = frozenset({"fallback", "pending", "excluded", "non_subject"})


def main() -> int:
    source_dir = pathlib.Path("src")
    if not source_dir.is_dir():
        print("diagnostic-coverage: src/ directory not found", file=sys.stderr)
        return 1

    headers = {p.name for p in source_dir.glob("cljonic-*.hpp")}

    failures: list[str] = []

    # Forward binding: every src/ header must be classified.
    unclassified = sorted(headers - REGISTRY.keys())
    for name in unclassified:
        failures.append(
            f"unclassified header {name}: add it to the registry as fallback, pending, excluded, or non_subject"
        )

    stale = sorted(REGISTRY.keys() - headers)
    for name in stale:
        failures.append(f"registry entry {name} has no matching src/ header")

    pending: list[str] = []
    for name, status in sorted(REGISTRY.items()):
        if status not in VALID_STATUSES:
            failures.append(f"{name}: invalid status {status!r}")
            continue
        path = source_dir / name
        if not path.is_file():
            continue
        text = path.read_text(encoding="utf-8")
        if status == "fallback" and FALLBACK_MARKER not in text:
            failures.append(f"{name}: classified as fallback but has no diagnostic-fallback mechanism ({FALLBACK_MARKER})")
        elif status == "pending":
            pending.append(name)

    if failures:
        for failure in failures:
            print(f"diagnostic-coverage: {failure}", file=sys.stderr)
        return 1

    if pending:
        print(f"diagnostic-coverage:pending={len(pending)} ({', '.join(pending)})")
    print("diagnostic-coverage:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
