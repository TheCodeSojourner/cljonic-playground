#!/usr/bin/env python3
"""Choose a conservative validation budget from available CPU and memory."""

from pathlib import Path
import subprocess


GIB = 1024 ** 3


def job_budget(cpus: int, available_bytes: int) -> int:
    if cpus < 1 or available_bytes < 0:
        raise ValueError("CPU count must be positive and available memory nonnegative")
    return max(1, min(cpus * 4 // 5, 12, available_bytes // GIB))


def available_memory() -> int:
    fields = dict(line.split(":", 1) for line in Path("/proc/meminfo").read_text().splitlines())
    available = int(fields["MemAvailable"].split()[0]) * 1024
    cgroup_root = Path("/sys/fs/cgroup")
    for line in Path("/proc/self/cgroup").read_text().splitlines():
        if not line.startswith("0::"):
            continue
        group = (cgroup_root / line[3:].lstrip("/")).resolve()
        if not group.is_relative_to(cgroup_root):
            raise ValueError("cgroup path is outside the cgroup mount")
        for directory in (group, *group.parents):
            if not directory.is_relative_to(cgroup_root):
                break
            limit_file = directory / "memory.max"
            if not limit_file.is_file():
                continue
            limit = limit_file.read_text().strip()
            if limit != "max":
                used = int((directory / "memory.current").read_text())
                available = min(available, max(0, int(limit) - used))
    return available


def default_jobs() -> int:
    cpus = int(subprocess.check_output(["nproc"], text=True).strip())
    return job_budget(cpus, available_memory())


if __name__ == "__main__":
    print(default_jobs())
