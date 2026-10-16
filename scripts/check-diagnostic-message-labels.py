#!/usr/bin/env python3
"""Diagnostic message-label gate for REQ-DIAG-011.

Every targeted compile-time diagnostic message in ``src/`` opens with a
fully-qualified label -- ``cljonic::`` + the public free-function or
class-template name + ``": "`` -- before a plain-language statement of the
violated constraint. The obligation is universal and forward-binding, and it
covers the class-internal capacity, initializer-count, constructor, range/view,
and static-extent assertions, not only the closed-domain rejection fallbacks of
``REQ-DIAG-009`` / ``REQ-DIAG-010``.

Statuses
--------
- ``labelled``: the message opens with the required label.
- ``pending``: parked in the recorded backlog below -- a message that still
  opens with a bare class name. Reported, not fatal, so the governance rule can
  land ahead of the application slice that relabels the messages. The backlog is
  currently empty, and a ``pending`` entry is expected to shrink away as the
  relabel lands.
- not listed and unlabelled: a conformance defect; the gate fails.

Out of scope
------------
Asserts inside ``/** */`` documentation example blocks assert example values and
carry no diagnostic message, so they are skipped. A rejection that carries no
message at all -- a class-template constraint already failing with one named
compiler diagnostic (``Range``, ``MapEntry``, ``Repeat``, ``Cycle``,
``Iterate``, ``Repeatedly``, ``Variant``) -- has no label to check.

The message is the trailing top-level argument, and the label must be carried by
that message's *first* string literal, because the five capacity messages
concatenate a fixed head with an interpolated tail (``"cljonic::Vector:
CapacityValue exceeds " "...=" CLJONIC_STRINGIFY(...)``).

A ``pending`` entry is keyed by (header, first literal). Relabelling or rewording
a pending message therefore removes it from the backlog and the gate reports the
entry as stale, so the backlog cannot silently outlive the fix.
"""

from __future__ import annotations

import pathlib
import re
import sys

LABEL_RE = re.compile(r"^cljonic::[A-Za-z_][A-Za-z0-9_]*: ")

# (header, first literal of the message) -> subject to REQ-DIAG-011, not yet
# relabelled. The application backlog is empty: the 24 class-internal collection
# assertions were relabelled when the rule landed. The mechanism stays so a
# future slice can park known unlabelled messages here instead of failing the
# gate, exactly as the sibling diagnostic-coverage gate does.
PENDING_MESSAGES: frozenset[tuple[str, str]] = frozenset()


def doc_comment_intervals(text: str) -> list[tuple[int, int]]:
    """Byte ranges of ``/** ... */`` documentation blocks."""
    intervals: list[tuple[int, int]] = []
    for match in re.finditer(r"/\*\*", text):
        end = text.find("*/", match.end())
        if end != -1:
            intervals.append((match.start(), end + 2))
    return intervals


def _skip_literal(text: str, index: int) -> int:
    """Index just past the string or character literal starting at ``index``."""
    quote = text[index]
    index += 1
    while index < len(text) and text[index] != quote:
        index += 2 if text[index] == "\\" else 1
    return index + 1


def statement_body(text: str, start: int) -> str:
    """Argument text of the ``static_assert(`` call whose arguments begin at ``start``."""
    depth = 1
    index = start
    while index < len(text) and depth > 0:
        char = text[index]
        if char in "\"'":
            index = _skip_literal(text, index)
            continue
        if char in "([{":
            depth += 1
        elif char in ")]}":
            depth -= 1
        index += 1
    return text[start : index - 1]


def top_level_commas(text: str) -> list[int]:
    """Offsets of commas outside any bracket nesting and outside literals."""
    commas: list[int] = []
    depth = 0
    index = 0
    while index < len(text):
        char = text[index]
        if char in "\"'":
            index = _skip_literal(text, index)
            continue
        if char in "([{":
            depth += 1
        elif char in ")]}":
            depth -= 1
        elif char == "," and depth == 0:
            commas.append(index)
        index += 1
    return commas


def message_head(statement: str) -> str | None:
    """First string literal of the trailing message argument, or ``None``."""
    commas = top_level_commas(statement)
    if not commas:
        return None
    message = statement[commas[-1] + 1 :].strip()
    if not message.startswith('"'):
        return None
    literals = re.findall(r'"((?:[^"\\]|\\.)*)"', message)
    return literals[0] if literals else None


def unlabelled_messages(source_dir: pathlib.Path) -> list[tuple[str, str, int]]:
    """``(header, first literal, line)`` for every message lacking the label."""
    found: list[tuple[str, str, int]] = []
    for path in sorted(source_dir.glob("cljonic-*.hpp")):
        text = path.read_text(encoding="utf-8")
        doc_intervals = doc_comment_intervals(text)
        for match in re.finditer(r"static_assert\s*\(", text):
            if any(start <= match.start() < end for start, end in doc_intervals):
                continue
            head = message_head(statement_body(text, match.end()))
            if head is None or LABEL_RE.match(head):
                continue
            found.append((path.name, head, text[: match.start()].count("\n") + 1))
    return found


def main() -> int:
    source_dir = pathlib.Path("src")
    if not source_dir.is_dir():
        print("diagnostic-message-labels: src/ directory not found", file=sys.stderr)
        return 1

    headers = {path.name for path in source_dir.glob("cljonic-*.hpp")}
    failures: list[str] = [
        f"backlog entry {header} has no matching src/ header" for header, _ in PENDING_MESSAGES if header not in headers
    ]

    found = unlabelled_messages(source_dir)
    seen = {(header, head) for header, head, _ in found}

    for header, head, line in found:
        if (header, head) in PENDING_MESSAGES:
            continue
        failures.append(
            f"{header}:{line}: message must open with a 'cljonic::<entity>: ' label (REQ-DIAG-011): {head!r}"
        )

    for header, head in sorted(PENDING_MESSAGES - seen):
        failures.append(
            f"backlog entry {header}: {head!r} is no longer an unlabelled message; remove it from PENDING_MESSAGES"
        )

    if failures:
        for failure in failures:
            print(f"diagnostic-message-labels: {failure}", file=sys.stderr)
        return 1

    if found:
        print(f"diagnostic-message-labels:pending={len(found)} (backlog: {'; '.join(sorted({h for h, _ in PENDING_MESSAGES}))})")
    print("diagnostic-message-labels:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
