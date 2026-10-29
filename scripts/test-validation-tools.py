#!/usr/bin/env python3
"""Regression tests for generated-header, diagnostic, and scheduling tools."""

import os
import argparse
from collections import Counter
from contextlib import redirect_stdout
import importlib.util
from io import StringIO
import json
from pathlib import Path
import re
import subprocess
import sys
import shlex
from tempfile import TemporaryDirectory
import unittest
from unittest.mock import patch
from threading import Barrier, Event, Lock

from amalgamate import AmalgamationError, generate
from compile_probe import compile_probe
from check_compile_fail_registry import registry_errors
from validation_commands import CommandResult, positive_jobs, report_results, run_commands
from validation_jobs import GIB, available_memory, default_jobs, job_budget


class ValidationBudgetTests(unittest.TestCase):
    def test_default_uses_available_cpu_detection_and_memory(self):
        with patch("validation_jobs.subprocess.check_output", return_value="10\n") as detect:
            with patch("validation_jobs.available_memory", return_value=6 * GIB):
                self.assertEqual(default_jobs(), 6)
        detect.assert_called_once_with(["nproc"], text=True)

    def test_make_preserves_explicit_environment_budget(self):
        root = Path(__file__).resolve().parent.parent
        result = subprocess.run(
            ["make", "--no-print-directory", "-s", "--eval",
             "_budget_test:;@echo $(VALIDATION_JOBS)", "_budget_test"],
            cwd=root, env={**os.environ, "VALIDATION_JOBS": "3"},
            text=True, capture_output=True, check=True,
        )
        self.assertEqual(result.stdout.strip(), "3")

    def test_cpu_ceiling_measured_cap_and_memory_headroom(self):
        for cpus, gib, expected in (
            (1, 16, 1), (2, 16, 1), (8, 16, 6), (22, 16, 12),
            (128, 128, 12), (22, 5, 5), (22, 0, 1),
        ):
            with self.subTest(cpus=cpus, gib=gib):
                self.assertEqual(job_budget(cpus, gib * GIB), expected)
        with self.assertRaises(ValueError):
            job_budget(0, GIB)
        with self.assertRaises(ValueError):
            job_budget(4, -1)

    def test_host_and_enclosing_cgroup_memory_limits(self):
        contents = {
            "/proc/meminfo": f"MemAvailable: {16 * GIB // 1024} kB\n",
            "/proc/self/cgroup": "0::/parent/child\n",
            "/sys/fs/cgroup/parent/child/memory.max": "max\n",
            "/sys/fs/cgroup/parent/memory.max": str(8 * GIB),
            "/sys/fs/cgroup/parent/memory.current": str(3 * GIB),
        }
        with patch.object(Path, "read_text", lambda path: contents[str(path)]):
            with patch.object(Path, "is_file", lambda path: str(path) in contents):
                self.assertEqual(available_memory(), 5 * GIB)
                contents["/sys/fs/cgroup/parent/memory.current"] = str(9 * GIB)
                self.assertEqual(available_memory(), 0)
                contents["/sys/fs/cgroup/parent/memory.max"] = "max"
                self.assertEqual(available_memory(), 16 * GIB)


class AmalgamationTests(unittest.TestCase):
    def test_unchanged_output_preserves_timestamp_inode_and_permissions(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "cljonic-core.hpp"
            source.write_text("#pragma once\n", encoding="utf-8")
            output = root / "output" / "cljonic.hpp"
            generate(root, source.name, output)
            output.chmod(0o640)
            os.utime(output, ns=(1_000_000_000, 1_000_000_000))
            before = output.stat()
            content = output.read_bytes()
            generate(root, source.name, output)
            after = output.stat()
            self.assertEqual(after.st_mtime_ns, before.st_mtime_ns)
            self.assertEqual(after.st_ino, before.st_ino)
            self.assertEqual(after.st_mode, before.st_mode)
            self.assertEqual(output.read_bytes(), content)

    def test_changed_output_is_replaced_and_preserves_permissions(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "cljonic-core.hpp"
            source.write_text("#pragma once\n", encoding="utf-8")
            output = root / "cljonic.hpp"
            generate(root, source.name, output)
            output.chmod(0o640)
            before = output.stat()
            source.write_text("#pragma once\n// changed\n", encoding="utf-8")
            generate(root, source.name, output)
            self.assertIn(b"// changed", output.read_bytes())
            self.assertNotEqual(output.stat().st_ino, before.st_ino)
            self.assertEqual(output.stat().st_mode, before.st_mode)
            self.assertEqual(sorted(path.name for path in root.iterdir()), ["cljonic-core.hpp", "cljonic.hpp"])

    def test_invalid_input_does_not_replace_existing_output(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "cljonic-core.hpp"
            source.write_text("#include <cljonic-missing.hpp>\n", encoding="utf-8")
            output = root / "cljonic.hpp"
            output.write_text("existing output\n", encoding="utf-8")
            with self.assertRaises(AmalgamationError):
                generate(root, source.name, output)
            self.assertEqual(output.read_text(), "existing output\n")


class CompileProbeTests(unittest.TestCase):
    def setUp(self):
        compile_probe.cache_clear()

    def tearDown(self):
        compile_probe.cache_clear()

    def test_identical_probes_reuse_failure_and_complete_diagnostic_output(self):
        result = subprocess.CompletedProcess(["c++"], 1, "stdout", "targeted diagnostic")
        with patch("compile_probe.subprocess.run", return_value=result) as run:
            first = compile_probe(("c++",), "src", "invalid source")
            second = compile_probe(("c++",), "src", "invalid source")
            run.assert_called_once_with(
                ["c++", "-std=c++23", "-fsyntax-only", "-I", "src", "-x", "c++", "-"],
                input="invalid source", text=True, capture_output=True, check=False,
            )
            self.assertIs(first, second)
            self.assertEqual((second.returncode, second.stdout, second.stderr), (1, "stdout", "targeted diagnostic"))

    def test_source_include_directory_compiler_and_flags_are_distinct(self):
        with patch("compile_probe.subprocess.run", return_value=subprocess.CompletedProcess([], 0)) as run:
            compile_probe(("c++",), "src", "source")
            compile_probe(("c++",), ".", "source")
            compile_probe(("clang++",), "src", "source")
            compile_probe(("c++", "-DCOUNT=0"), "src", "source")
            compile_probe(("c++",), "src", "different source")
            self.assertEqual(run.call_count, 5)

    def test_execution_errors_are_not_cached(self):
        with patch("compile_probe.subprocess.run", side_effect=FileNotFoundError("compiler missing")) as run:
            for _ in range(2):
                with self.assertRaises(FileNotFoundError):
                    compile_probe(("missing",), "src", "source")
            self.assertEqual(run.call_count, 2)


class ParallelCommandTests(unittest.TestCase):
    def test_worker_limit_and_ordered_results(self):
        barrier = Barrier(2, timeout=5)
        lock = Lock()
        active = 0
        maximum = 0

        def execute(label, command):
            nonlocal active, maximum
            with lock:
                active += 1
                maximum = max(maximum, active)
            barrier.wait()
            with lock:
                active -= 1
            return CommandResult(label, 0, label + "\n", 0)

        commands = [(str(i), ["unused"]) for i in range(6)]
        with patch("validation_commands.execute_command", side_effect=execute):
            results = run_commands(commands, 2)
        self.assertEqual(maximum, 2)
        self.assertEqual([result.label for result in results], [str(i) for i in range(6)])

    def test_failure_stops_new_work_and_is_reported(self):
        failure = CommandResult("failure", 7, "compiler error\n", 0)
        with patch("validation_commands.execute_command", return_value=failure) as execute:
            results = run_commands([("failure", []), ("must-not-run", [])], 1)
            self.assertEqual(execute.call_count, 1)
        output = StringIO()
        with redirect_stdout(output):
            self.assertFalse(report_results(results))
        self.assertIn("compiler error", output.getvalue())
        self.assertIn("failed exit=7", output.getvalue())

    def test_out_of_order_completion_keeps_output_order(self):
        second_finished = Event()

        def execute(label, command):
            if label == "first":
                if not second_finished.wait(timeout=5):
                    raise TimeoutError("second command did not run")
            else:
                second_finished.set()
            return CommandResult(label, 0, label + "\n", 0)

        with patch("validation_commands.execute_command", side_effect=execute):
            results = run_commands([("first", []), ("second", [])], 2)
        self.assertEqual([result.label for result in results], ["first", "second"])

    def test_failure_drains_active_work_without_starting_queued_work(self):
        from concurrent.futures import wait as wait_for_commands

        started = Barrier(2, timeout=5)
        failure_observed = Event()

        def execute(label, command):
            started.wait()
            if label == "active" and not failure_observed.wait(timeout=5):
                raise TimeoutError("failure was not observed")
            return CommandResult(label, 1 if label == "failure" else 0, label + "\n", 0)

        def wait_for_failure(*args, **kwargs):
            completed = wait_for_commands(*args, **kwargs)
            failure_observed.set()
            return completed

        with patch("validation_commands.execute_command", side_effect=execute):
            with patch("validation_commands.wait", side_effect=wait_for_failure):
                results = run_commands(
                    [("failure", []), ("active", []), ("must-not-run", [])], 2,
                )
        self.assertEqual([result.label for result in results], ["failure", "active"])

    def test_empty_work_and_invalid_limits(self):
        self.assertEqual(run_commands([], 2), [])
        with self.assertRaises(ValueError):
            run_commands([], 0)
        for value in ("0", "-1", "bad"):
            with self.assertRaises(argparse.ArgumentTypeError):
                positive_jobs(value)
        self.assertEqual(positive_jobs("4"), 4)

    def test_execution_exception_is_not_success(self):
        with patch("validation_commands.execute_command", side_effect=FileNotFoundError("missing tool")):
            with self.assertRaises(FileNotFoundError):
                run_commands([("missing", [])], 2)

    def test_primitive_partitions_preserve_every_probe(self):
        path = Path(__file__).with_name("check-primitive-compile-failures.py")
        spec = importlib.util.spec_from_file_location("primitive_checks", path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        result = subprocess.CompletedProcess([], 1, "", "")

        def probes(arguments):
            with patch.object(sys, "argv", [str(path), *arguments]):
                with patch.object(module, "compile_source", return_value=result) as compile_source:
                    with redirect_stdout(StringIO()):
                        module.main()
            return Counter((call.args[1], call.args[2]) for call in compile_source.call_args_list)

        complete = probes([])
        partitioned = Counter()
        for primitive in module.PRIMITIVES:
            partitioned.update(probes(["--primitive", primitive]))
        self.assertEqual(partitioned, complete)
        expected_count = 2 * sum(
            len(cases["diagnostic_cases"]) + len(cases["pass_cases"])
            for cases in module.PRIMITIVES.values()
        )
        self.assertEqual(complete.total(), expected_count)

    def run_dispatcher(self, *arguments):
        fake_make = shlex.join([
            sys.executable, "-c",
            "import sys; print(sys.argv[-1]); sys.exit(7 if sys.argv[-1] == 'failure' else 0)",
        ])
        return subprocess.run(
            [sys.executable, str(Path(__file__).with_name("run-compile-fail.py")),
             "--make", fake_make, *arguments],
            text=True, capture_output=True, check=False,
        )

    def test_dispatcher_failure_prevents_success_and_later_commands(self):
        result = self.run_dispatcher("--jobs", "1", "--targets", "failure", "must-not-run")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("failed exit=7", result.stdout)
        self.assertNotIn("must-not-run", result.stdout)
        self.assertNotIn("compile-fail:ok", result.stdout)

    def test_dispatcher_discovers_all_primitive_groups_and_future_targets(self):
        result = self.run_dispatcher(
            "--jobs", "2", "--targets", "primitive-compile-fail", "future-api-compile-fail",
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        listing = subprocess.run(
            [sys.executable, str(Path(__file__).with_name("check-primitive-compile-failures.py")),
             "--list-primitives"],
            text=True, capture_output=True, check=True,
        )
        expected = listing.stdout.splitlines()
        actual = [
            line.removeprefix("PRIMITIVE=")
            for line in result.stdout.splitlines() if line.startswith("PRIMITIVE=")
        ]
        self.assertEqual(actual, expected)
        self.assertEqual(result.stdout.count("validation:primitive-compile-fail/"), len(expected))
        self.assertIn("validation:future-api-compile-fail:ok", result.stdout)
        self.assertIn("compile-fail:ok", result.stdout)

    def test_dispatcher_rejects_invalid_worker_count(self):
        result = self.run_dispatcher("--jobs", "0", "--targets", "must-not-run")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("jobs must be a positive integer", result.stderr)
        self.assertNotIn("validation:", result.stdout)


class CompileFailRegistryTests(unittest.TestCase):
    MAKEFILE = (
        ".PHONY: alpha-compile-fail beta-compile-fail\n"
        "# ignored-compile-fail:\n"
        "alpha-compile-fail: cljonic\n"
        "\t@echo fake-compile-fail:\n"
        "beta-compile-fail: script.py\n"
    )

    def test_complete_registry(self):
        self.assertEqual(registry_errors(self.MAKEFILE, ["alpha-compile-fail", "beta-compile-fail"]), [])

    def test_missing_registration(self):
        self.assertEqual(
            registry_errors(self.MAKEFILE, ["alpha-compile-fail"]),
            ["unregistered target: beta-compile-fail"],
        )

    def test_duplicate_registration(self):
        self.assertEqual(
            registry_errors(self.MAKEFILE, ["alpha-compile-fail", "beta-compile-fail", "alpha-compile-fail"]),
            ["duplicate registration: alpha-compile-fail"],
        )

    def test_stale_registration(self):
        self.assertEqual(
            registry_errors(self.MAKEFILE, ["alpha-compile-fail", "beta-compile-fail", "stale-compile-fail"]),
            ["registration has no target declaration: stale-compile-fail"],
        )

    def test_empty_registry_fails(self):
        self.assertEqual(len(registry_errors(self.MAKEFILE, [])), 2)

    def test_multiple_targets_and_continuations(self):
        self.assertEqual(
            registry_errors("alpha-compile-fail \\\n beta-compile-fail: cljonic\n",
                            ["alpha-compile-fail", "beta-compile-fail"]),
            [],
        )


class GitGateSchedulingTests(unittest.TestCase):
    def run_gate(self, failure="", jobs="2"):
        root = Path(__file__).resolve().parent.parent
        makefile = (root / "Makefile").read_text()
        targets = [
            "requirements-structure", "vocabulary-structure", "audit-scripts",
            "compile-fail-registry", "format", "cljonic", "configure",
            "lint-check", "complexity-cli", "header-guards", "diagnostic-coverage",
            "diagnostic-message-labels", "traceability-spec-to-code", "docs",
            "docs-examples", "sanitizer-cli", "coverage-cli", "no-heap", "cljonic-test",
            *re.findall(r"^([a-z0-9-]+-compile-fail):", makefile, re.MULTILINE),
        ]
        with TemporaryDirectory() as directory:
            path = Path(directory)
            log = path / "events.jsonl"
            helper = path / "record.py"
            helper.write_text(
                "import json, os, sys\n"
                "target, jobs = sys.argv[1:]\n"
                "with open(os.environ['GATE_LOG'], 'a') as log:\n"
                "    log.write(json.dumps([target, jobs]) + '\\n')\n"
                "sys.exit(1 if target == os.environ['GATE_FAILURE'] else 0)\n",
                encoding="utf-8",
            )
            overrides = path / "overrides.mk"
            overrides.write_text("".join(
                f"{target}:\n\t@{shlex.quote(sys.executable)} {shlex.quote(str(helper))} "
                f"{target} '$(BUILD_JOBS)'\n"
                for target in targets
            ), encoding="utf-8")
            make = ["make", "-f", str(root / "Makefile"), "-f", str(overrides)]
            result = subprocess.run(
                [*make, "--no-print-directory", "-s", "-j8", "git",
                 f"MAKE={shlex.join(make)}", f"VALIDATION_JOBS={jobs}"],
                cwd=root, env={**os.environ, "GATE_LOG": str(log), "GATE_FAILURE": failure},
                text=True, capture_output=True, check=False,
            )
            events = [json.loads(line) for line in log.read_text().splitlines()] if log.exists() else []
        return result, events

    def test_graph_orders_mutation_readers_and_exclusive_profiles(self):
        result, events = self.run_gate()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        names = [name for name, _ in events]
        required = {
            "requirements-structure", "vocabulary-structure", "audit-scripts",
            "compile-fail-registry", "format", "cljonic", "configure",
            "lint-check", "complexity-cli", "header-guards", "diagnostic-coverage",
            "diagnostic-message-labels", "traceability-spec-to-code", "docs",
            "docs-examples", "sanitizer-cli", "coverage-cli", "no-heap", "cljonic-test",
        }
        makefile = Path(__file__).resolve().parent.parent / "Makefile"
        diagnostic_targets = re.findall(
            r"^([a-z0-9-]+-compile-fail):", makefile.read_text(), re.MULTILINE,
        )
        required.update(diagnostic_targets)
        self.assertEqual(set(names), required)
        for target in required - {"cljonic", "primitive-compile-fail"}:
            self.assertEqual(names.count(target), 1, target)
        listing = subprocess.run(
            [sys.executable, str(Path(__file__).with_name("check-primitive-compile-failures.py")),
             "--list-primitives"],
            text=True, capture_output=True, check=True,
        )
        self.assertEqual(names.count("primitive-compile-fail"), len(listing.stdout.splitlines()))
        self.assertEqual(names.count("format"), 1)
        self.assertLess(names.index("compile-fail-registry"), names.index("format"))
        for name in ("lint-check", "docs-examples", "primitive-compile-fail", "docs"):
            self.assertLess(names.index("format"), names.index(name))
            self.assertLess(names.index("cljonic"), names.index(name))
        profiles = ["sanitizer-cli", "coverage-cli", "no-heap", "cljonic-test"]
        exclusive = [name for name in names[names.index("sanitizer-cli"):] if name != "cljonic"]
        self.assertEqual(exclusive, profiles)
        for name, jobs in events:
            if name in profiles[:-1]:
                self.assertEqual(jobs, "2")
            if name in ("lint-check", "docs-examples"):
                self.assertEqual(jobs, "1")
        self.assertIn("git:ok", result.stdout)

    def test_failure_blocks_dependent_stages_and_success(self):
        for failure, forbidden in (
            ("compile-fail-registry", "format"),
            ("format", "lint-check"),
            ("lint-check", "sanitizer-cli"),
            ("map-entry-factory-compile-fail", "sanitizer-cli"),
            ("docs-examples", "sanitizer-cli"),
            ("sanitizer-cli", "coverage-cli"),
            ("coverage-cli", "no-heap"),
            ("no-heap", "cljonic-test"),
            ("cljonic-test", "must-not-run"),
        ):
            with self.subTest(failure=failure):
                result, events = self.run_gate(failure=failure)
                self.assertNotEqual(result.returncode, 0)
                self.assertNotIn(forbidden, [name for name, _ in events])
                self.assertNotIn("git:ok", result.stdout)

    def test_invalid_budget_fails_before_any_work(self):
        result, events = self.run_gate(jobs="0")
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(events, [])
        self.assertNotIn("git:ok", result.stdout)


if __name__ == "__main__":
    unittest.main()
