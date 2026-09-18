#!/usr/bin/env python3
"""Verify Markdown structure in cljonic-core.hpp's Core_Cheatsheet block."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


ANCHOR = r"\anchor Core_Cheatsheet"
COMMENT_LINE = re.compile(r"^\s*\* ?(.*)$")


def cheatsheet_lines(source: str) -> list[tuple[int, str]]:
    anchor = source.find(ANCHOR)
    if anchor < 0:
        raise ValueError("missing \\anchor Core_Cheatsheet")

    comment_end = source.find("*/", anchor)
    if comment_end < 0:
        raise ValueError("unterminated Doxygen comment after Core_Cheatsheet")

    before_anchor = source[:anchor]
    first_line = before_anchor.count("\n") + 1
    return [(first_line + offset, line) for offset, line in enumerate(source[anchor:comment_end].splitlines())]


def validate(source: str) -> list[str]:
    try:
        lines = cheatsheet_lines(source)
    except ValueError as error:
        return [str(error)]

    errors: list[str] = []
    contents = []
    for line_number, line in lines:
        match = COMMENT_LINE.match(line)
        if match:
            contents.append((line_number, match.group(1)))

    rule_index = next((index for index, (_, text) in enumerate(contents) if text == "___"), None)
    if rule_index is None:
        errors.append("missing standalone horizontal rule `* ___`")
    elif rule_index + 1 == len(contents) or contents[rule_index + 1][1] != "# Cheatsheet":
        errors.append(f"line {contents[rule_index][0]}: `* ___` must be followed by standalone `* # Cheatsheet`")

    table_lines = [(line_number, text) for line_number, text in contents if "|" in text]
    if len(table_lines) < 3:
        errors.append("aliases table requires separate header, separator, and data-row lines")
    else:
        for line_number, text in table_lines:
            if text.count("|") != 1:
                errors.append(f"line {line_number}: table content must occupy one source line")
        separator_index = next(
            (index for index, (_, text) in enumerate(table_lines) if re.fullmatch(r"[- |]+", text)), None
        )
        if separator_index != 1:
            errors.append("aliases table separator must be the line immediately after its header")

    for line_number, text in contents:
        if len(re.findall(r"(?:^|\s)-\s+\\ref\b", text)) > 1:
            errors.append(f"line {line_number}: each Markdown list item must occupy its own source line")

    return errors


def run_self_test(source: str) -> list[str]:
    errors = validate(source)
    if errors:
        return [f"canonical source failed validation: {error}" for error in errors]

    collapsed = source.replace(" * ___\n * # Cheatsheet", " * ___ # Cheatsheet", 1)
    if not validate(collapsed):
        return ["negative regression did not reject a collapsed cheatsheet heading"]
    return []


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", nargs="?", default="src/cljonic-core.hpp")
    parser.add_argument("--self-test", action="store_true", help="verify a deliberately collapsed heading is rejected")
    arguments = parser.parse_args()

    source_path = Path(arguments.source)
    errors = validate(source_path.read_text())
    if arguments.self_test and not errors:
        errors = run_self_test(source_path.read_text())

    if errors:
        print("core-cheatsheet-format:failed")
        print("\n".join(f"  {error}" for error in errors))
        return 1

    print("core-cheatsheet-format:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())