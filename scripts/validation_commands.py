"""Bounded subprocess scheduling with ordered output and failure propagation."""

import argparse
from concurrent.futures import FIRST_COMPLETED, ThreadPoolExecutor, wait
from dataclasses import dataclass
import subprocess
import time


def positive_jobs(value: str) -> int:
    try:
        jobs = int(value)
    except ValueError as error:
        raise argparse.ArgumentTypeError("jobs must be a positive integer") from error
    if jobs < 1:
        raise argparse.ArgumentTypeError("jobs must be a positive integer")
    return jobs


@dataclass
class CommandResult:
    label: str
    returncode: int
    output: str
    seconds: float


def execute_command(label: str, command: list[str]) -> CommandResult:
    start = time.monotonic()
    result = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=False)
    return CommandResult(label, result.returncode, result.stdout, time.monotonic() - start)


def run_commands(commands: list[tuple[str, list[str]]], jobs: int) -> list[CommandResult]:
    if jobs < 1:
        raise ValueError("jobs must be positive")
    results = {}
    next_index = 0
    failed = False
    with ThreadPoolExecutor(max_workers=jobs) as executor:
        pending = {}
        while pending or (not failed and next_index < len(commands)):
            while not failed and len(pending) < jobs and next_index < len(commands):
                label, command = commands[next_index]
                pending[executor.submit(execute_command, label, command)] = next_index
                next_index += 1
            done, _ = wait(pending, return_when=FIRST_COMPLETED)
            for future in done:
                index = pending.pop(future)
                result = future.result()
                results[index] = result
                failed = failed or result.returncode != 0
    return [results[index] for index in sorted(results)]


def report_results(results: list[CommandResult], quiet: bool = False) -> bool:
    all_ok = all(result.returncode == 0 for result in results)
    for result in results:
        failed = result.returncode != 0
        if quiet and not failed:
            continue
        if result.output:
            print(result.output, end="" if result.output.endswith("\n") else "\n")
        status = "failed exit=" + str(result.returncode) if failed else "ok"
        print(f"validation:{result.label}:{status}:seconds={result.seconds:.3f}", flush=True)
    return all_ok
