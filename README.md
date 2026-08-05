# cljonic

This repository tracks the durable source of truth for cljonic:

- architecture and vocabulary documents
- behavioral specifications under `specs/`
- test source files under `tests/`
- production source files under `src/`
- build configuration such as `CMakeLists.txt`

Generated local build output is intentionally not checked in.

## Clean Start

### Prerequisites

- GNU Make
- CMake 3.28 or newer
- A C++ compiler with C++23 support or better
- Network access during the first configure so CMake can fetch Catch2

## Primary Workflow

The repository provides a single root `Makefile`.

### Show Available Targets

```sh
make
```

This prints a short list of the supported Make targets and what each one does.

### Rebuild Everything And Run Tests

```sh
make all
```

This runs the full workflow. It removes local generated build output, configures the project, rebuilds the test suite from a clean state, and runs all discovered tests.

### Incremental Test Run

```sh
make test
```

This reuses the existing build tree, recompiles only what has changed, and then runs the test suite.

### Clean Local Build Output

```sh
make clean
```

This removes generated local build directories so the repository is ready for review or commit without checked-in build artifacts.

## Under The Hood

The `make` targets drive the existing CMake test harness. The first configure creates the local `build/` directory and fetches Catch2 automatically through CMake `FetchContent`.

## What Belongs In Git

Check in:

- hand-authored files in `src/`, `tests/`, `specs/`, and project documentation
- build configuration files such as `CMakeLists.txt`

Do not check in:

- `build/` or other CMake build directories
- fetched third-party content under CMake `_deps/`
- local test discovery output, compiler objects, or generated binaries

## Current Test Harness Behavior

- The default test run always executes the executable spec-model tests.
- If a production vector header is added later at `src/cljonic/vector.hpp`, integration coverage can be enabled automatically by the existing test harness without changing the bootstrap steps.