#!/usr/bin/env python3
"""Audit explicit compile-fail registrations against literal Make target declarations."""

import argparse
from collections import Counter
from pathlib import Path
import re
import sys


def registry_errors(makefile: str, targets: list[str]) -> list[str]:
    declarations = set()
    logical_lines = re.sub(r"\\\n[ \t]*", " ", makefile)
    for line in logical_lines.splitlines():
        if not line or line[0].isspace() or line.startswith("#"):
            continue
        match = re.match(r"([^:=]+):(?!=)", line)
        if match:
            declarations.update(
                name for name in match[1].split()
                if re.fullmatch(r"[a-z0-9-]+-compile-fail", name)
            )
    counts = Counter(targets)
    errors = [f"duplicate registration: {name}" for name, count in sorted(counts.items()) if count > 1]
    errors.extend(f"unregistered target: {name}" for name in sorted(declarations - counts.keys()))
    errors.extend(f"registration has no target declaration: {name}" for name in sorted(counts.keys() - declarations))
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--makefile", type=Path, required=True)
    parser.add_argument("--targets", nargs="*", required=True)
    args = parser.parse_args()
    errors = registry_errors(args.makefile.read_text(encoding="utf-8"), args.targets)
    if errors:
        for error in errors:
            print(f"compile-fail-registry: {error}", file=sys.stderr)
        return 1
    print(f"compile-fail-registry:ok targets={len(args.targets)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
