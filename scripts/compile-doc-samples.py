#!/usr/bin/env python3

import argparse
import pathlib
import re
import shutil
import sys
import textwrap

from validation_commands import positive_jobs, report_results, run_commands
from validation_jobs import default_jobs


BLOCK_OPEN_RE = re.compile(r"^[ \t]*~~~~~\{\.cpp\}[ \t]*$", re.MULTILINE)
BLOCK_CLOSE_RE = re.compile(r"^[ \t]*~~~~~[ \t]*$", re.MULTILINE)

# Example-content rule: every doc C++ sample must bring cljonic names into scope
# with `using namespace cljonic;` at file scope (after the #include, outside
# main()). The extraction dedents a block, so an in-main occurrence is the only
# indented one and the top-level occurrence is the only column-zero one.
USING_TOP_LEVEL_RE = re.compile(r"(?m)^using namespace cljonic;[ \t]*$")
USING_INDENTED_RE = re.compile(r"(?m)^[ \t]+using namespace cljonic;")

# Doc examples are compiled against the public single-header API (cljonic.hpp),
# which is generated from the cljonic-core.hpp umbrella. The remaining
# sequence-conversion free function is intentionally NOT part of that
# umbrella: `seq` remains deferred.
# Their doc examples therefore cannot be compiled against a public API they do
# not yet belong to. This exclusion is explicit (and reported) rather than
# silent; drop a header from this set only when it is promoted into
# cljonic-core.hpp.
DEFERRED_NON_PUBLIC_HEADERS = frozenset(
    {
        "cljonic-seq.hpp",
    }
)


def extract_cpp_blocks(content: str) -> list[str]:
    lines = content.splitlines()
    blocks: list[str] = []
    inside = False
    current: list[str] = []

    for line in lines:
        if not inside and BLOCK_OPEN_RE.match(line):
            inside = True
            current = []
            continue
        if inside and BLOCK_CLOSE_RE.match(line):
            blocks.append(textwrap.dedent("\n".join(current)).strip("\n") + "\n")
            inside = False
            current = []
            continue
        if inside:
            current.append(line)

    return blocks


def example_style_error(header_name: str, block_index: int, snippet: str) -> str | None:
    if USING_INDENTED_RE.search(snippet):
        return (
            f"{header_name} example {block_index}: 'using namespace cljonic;' must be at file scope "
            "(outside main()), not indented inside main()"
        )
    if not USING_TOP_LEVEL_RE.search(snippet):
        return (
            f"{header_name} example {block_index}: missing 'using namespace cljonic;' at file scope "
            "(after the #include, outside main())"
        )
    return None


def rewrite_public_header_include(snippet: str, header: str) -> str:
    header_name = pathlib.Path(header).name
    snippet = re.sub(
        r'^[ \t]*#include[ \t]+"cljonic-core\.hpp"[ \t]*$',
        f'#include "{header_name}"',
        snippet,
        flags=re.MULTILINE,
    )
    snippet = re.sub(
        r'^[ \t]*#include[ \t]+<cljonic-core\.hpp>[ \t]*$',
        f'#include <{header_name}>',
        snippet,
        flags=re.MULTILINE,
    )
    return snippet


def compile_command(cxx: str, source_file: pathlib.Path, object_file: pathlib.Path, include_root: pathlib.Path) -> list[str]:
    return [
        cxx,
        "-std=c++23",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-I",
        str(include_root),
        "-c",
        str(source_file),
        "-o",
        str(object_file),
    ]


def main() -> int:
    parser = argparse.ArgumentParser(description="Compile Doxygen C++ samples against generated cljonic header")
    parser.add_argument("--source-dir", default="src", help="Directory containing documented headers")
    parser.add_argument("--build-dir", default="build/docs-examples", help="Directory for generated sample translation units")
    parser.add_argument("--header", default="cljonic.hpp", help="Generated single-header artifact")
    parser.add_argument("--jobs", type=positive_jobs)
    args = parser.parse_args()
    if args.jobs is None:
        args.jobs = default_jobs()

    source_dir = pathlib.Path(args.source_dir)
    build_dir = pathlib.Path(args.build_dir)
    header_path = pathlib.Path(args.header)
    repo_root = pathlib.Path.cwd()

    cxx = shutil.which("g++") or shutil.which("clang++")
    if cxx is None:
        print("docs-examples: missing required compiler (g++ or clang++)", file=sys.stderr)
        return 1

    if not source_dir.is_dir():
        print(f"docs-examples: source directory not found: {source_dir}", file=sys.stderr)
        return 1

    if not (repo_root / header_path).is_file():
        print(f"docs-examples: generated header not found: {header_path}", file=sys.stderr)
        return 1

    build_dir.mkdir(parents=True, exist_ok=True)

    headers = sorted(source_dir.glob("*.hpp"))
    extracted: list[tuple[pathlib.Path, list[str]]] = [
        (header_file, extract_cpp_blocks(header_file.read_text(encoding="utf-8"))) for header_file in headers
    ]

    style_errors = [
        error
        for header_file, blocks in extracted
        for idx, block in enumerate(blocks, start=1)
        if (error := example_style_error(header_file.name, idx, block)) is not None
    ]
    if style_errors:
        for error in style_errors:
            print(f"docs-examples:{error}", file=sys.stderr)
        return 1

    example_count = 0
    commands = []
    deferred_skipped: list[str] = []
    for header_file, blocks in extracted:
        if header_file.name in DEFERRED_NON_PUBLIC_HEADERS:
            deferred_skipped.append(header_file.name)
            continue
        for idx, block in enumerate(blocks, start=1):
            snippet = rewrite_public_header_include(block, str(header_path))
            sample_base = f"{header_file.stem}-example-{idx}"
            sample_cpp = build_dir / f"{sample_base}.cpp"
            sample_obj = build_dir / f"{sample_base}.o"
            sample_cpp.write_text(snippet, encoding="utf-8")
            commands.append((sample_base, compile_command(cxx, sample_cpp, sample_obj, repo_root)))
            example_count += 1

    if example_count == 0:
        print("docs-examples: no C++ sample blocks found", file=sys.stderr)
        return 1

    if not report_results(run_commands(commands, args.jobs)):
        return 1

    if deferred_skipped:
        print(
            "docs-examples:deferred-skipped="
            f"{len(deferred_skipped)} ({', '.join(deferred_skipped)})"
        )
    print(f"docs-examples:compiled={example_count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
