#!/usr/bin/env python3
"""Run registered diagnostics and optional independent checks in one bounded pool."""

import argparse
import shlex
import subprocess
import sys

from validation_commands import positive_jobs, report_results, run_commands
from validation_jobs import default_jobs


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--jobs", type=positive_jobs)
    parser.add_argument("--make", default="make")
    parser.add_argument("--targets", nargs="+", required=True)
    parser.add_argument("--checks", nargs="*", default=[])
    args = parser.parse_args()
    if args.jobs is None:
        args.jobs = default_jobs()
    commands = []
    for target in args.targets:
        base = [*shlex.split(args.make), "--no-print-directory", "-s", "-j1", target]
        if target == "primitive-compile-fail":
            listing = subprocess.run(
                [sys.executable, "scripts/check-primitive-compile-failures.py", "--list-primitives"],
                text=True, capture_output=True, check=True,
            )
            primitives = listing.stdout.splitlines()
            if not primitives:
                parser.error("primitive inventory is empty")
            for primitive in primitives:
                commands.append((f"{target}/{primitive}", [*base, f"PRIMITIVE={primitive}"]))
        else:
            commands.append((target, base))
    for target in args.checks:
        commands.append((target, [
            *shlex.split(args.make), "--no-print-directory", "-s", "-j1", target,
            "VALIDATION_JOBS=1", "BUILD_JOBS=1",
        ]))
    print(f"compile-fail:running groups={len(commands)} jobs={args.jobs}", flush=True)
    if not report_results(run_commands(commands, args.jobs)):
        return 1
    print("compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
