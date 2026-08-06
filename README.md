# cljonic

<p align="center">
  <img src="logo.png" width="200" height="200" alt="cljonic" />
</p>

<p align="center"><strong>Header-Only Embedded-Systems C++ Clojure-esque Functional Programming Library</strong></p>

> **Note**
> If you are interested in a "big-machine" **C++ Clojure** implementation see the [Jank](https://github.com/jank-lang/jank) project.

> **Important**
> I am **NOT** a **Modern C++** expert. I'm learning **Modern C++** (e.g., **Concepts**, **Reflection**) using **AI-assisted workflows**. I'm moderately competent in **Clojure**, having used it professionally for several years. If you have suggestions for improvement, **PLEASE** feel free to create issues, and I'll **do my best** to address them. **PRs** are also welcome.

## Pronunciation

**cljonic** /klə-jŏn′ĭk/

## Obligatory Word Definition

**Laconic**

- Using very few words; concise to the point of seeming mysterious or rude.

## Detailed Documentation

> **Note**
> Detailed documentation is coming soon.

## Motivation

**John Carmack** - https://www.gamedeveloper.com/programming/in-depth-functional-programming-in-c-

> "A large fraction of the flaws in software development are due to programmers not fully understanding all the
> possible states their code may execute in. ... Programming in a functional style makes the state presented to your code
> explicit, which makes it much easier to reason about [(i.e., Fewer mistakes, less debugging, and more AI manageable)] ...
>
> No matter what language you work in, programming in a functional style provides benefits. You should do it whenever it
> is convenient, and you should think hard about the decision when it isn't convenient."

**Clojure** is one of the most expressive and concise programming languages. It is **laconic** in nature,
**immutable-by-default**, **practical**, and **stable**. **Clojure** is based on the **Lisp** programming language. However,
**Clojure** is not well suited for some programming domains, such as **embedded systems** with tight memory and CPU
constraints that do not allow **heap** usage and only offer **C++** programming tools.

**C++**, on the other hand, is quite different from **Clojure**. It tends to be **verbose** rather than **laconic**. **C++**
is not immutable-by-default and is not considered a functional programming language, although it has functional
features like **lambdas** and standard libraries such as **ranges**, **views**, **optional**, and **expected**. **C++** is a
**BIG** language that is constantly evolving, with a large and growing feature set. It offers many ways to do things,
some of which make it **easy to do things poorly**.

Most **embedded system developers** are not computer language experts. They do not have time to keep up with the latest
**C++** language features, and their educational background is often not heavy in the nuances of optimal **Modern**
**C++** usage. When it comes to **Modern** **C++**, they are often mostly **self-taught** and have learned on the job.

Some **embedded system** projects do not require heavy use of the **C++** features designed for memory and CPU utilization
optimization. Many projects have a small amount of code that must be **finely tuned**, and quite a bit of code that is
**not performance critical**.

The motivation behind **cljonic** is to provide a **Clojure-esque** functional programming library for **C++** embedded
systems developers. It aims to be **laconic**, **immutable-by-default**, **practical**, and **stable**. **Cljonic** should also be
**easy to learn and understand**, and **easy to use properly** for the **safe** and **efficient** implementation of embedded
systems.

## Desired cljonic User Experience

The developer using **cljonic** should be able to write **safe** code that is **easy to read**, **easy to understand** and
**easy to maintain**. These goals will enable **embedded system** projects to be **completed more quickly**, with
**fewer bugs**, and maintained and enhanced with **less long term effort**.

### Default Element

Every **cljonic collection** has a **default element** that is returned whenever a valid collection element can't be returned.
The value of a collection's **default element** is a default initialized instance of the type of the collection's values
(e.g., T{}). This approach is taken to avoid using exceptions, error codes/states, `std::optional`, and/or `std::expected`.

> **Note**
> The design choice to use a **default element** means that **cljonic** users must take care to ensure that they know
> when a **default element** can be returned (e.g., Asking for the First element of an empty collection), and to handle things appropriately.

### C++ constexpr and consteval

**cljonic** uses **constexpr** and **consteval** whenever possible to enable/require **compile-time** evaluation of expressions.
Additionally, this approach helps to detect **undefined behavior** at compile time.

### concepts

**cljonic** uses **concepts** to specify template requirements, improving code readability and providing better compiler diagnostics.

## Desired cljonic Maintainer Experience

**cljonic** has a **Makefile** in the root directory. For **help** on the available development processes simply execute
`make` from a command-line while in the root directory to generate a list of the available options.

**After cloning the repository**, the developer should first execute `make all`. Next, assuming no files are added to the
repository, only modified, the developer should execute `make test`. Assuming all tests pass, executing `make cljonic`
*(coming soon)* will generate the **cljonic** single header file. Finally, executing `make git` *(coming soon)* will
prepare the repository for a **git** commit.

> **Note**
> Whenever new files are added to the `src/` or `tests/` directories the developer must run `make all` again.

### Prerequisites

- GNU Make
- CMake 3.28 or newer
- A C++ compiler with C++23 support or better
- Network access during the first configure so CMake can fetch Catch2

### Available Targets

| Target                    | Description                                                                                                            |
| ------------------------- | ---------------------------------------------------------------------------------------------------------------------- |
| `make`                    | Show available targets                                                                                                 |
| `make all`                | Clean, configure, parallel rebuild, and parallel test run                                                              |
| `make test`               | Incremental parallel rebuild and parallel test run                                                                     |
| `make clean`              | Remove generated local build output                                                                                    |
| `make coverage`           | Build with instrumentation, run tests, enforce 100% line coverage                                                      |
| `make coverage-cli`       | Same as coverage but print lines % to stdout; set `COVERAGE_FILE=foo.hpp` to narrow scope                              |
| `make sanitizer`          | Build with ASan+UBSan and run tests                                                                                    |
| `make sanitizer-cli`      | Quiet ASan+UBSan run for loops; prints `sanitizer:ok` on pass                                                          |
| `make complexity`         | Run lizard on `COMPLEXITY_PATH` (default `src`); set `CYCLOMATIC_COMPLEXITY_THRESHOLD` and `FUNCTION_LENGTH_THRESHOLD` |
| `make complexity-cli`     | Quiet lizard warning-only check on `COMPLEXITY_PATH`; fails if thresholds are exceeded                                 |
| `make format`             | Format all source and test C/C++ files in place with clang-format                                                      |
| `make lint`               | Run clang-format and clang-tidy checks; set `LINT_FILE=src/foo.hpp` or `tests/bar.cpp` to narrow scope                 |
| `make no-heap-src`        | Fail if `src` contains common heap-allocation APIs or heap-backed STL containers                                       |
| `make no-heap-symbols`    | Fail if compiled artifact contains forbidden allocator symbols                                                         |
| `make no-heap`            | Strict no-heap gate: source check, harness build, and binary symbol scan                                               |
| `make upsert-gate`        | Fail-fast loop gate: lint, complexity-cli, asan-ubsan, coverage-cli for `UPSERT_COVERAGE_FILE`                         |
| `make upsert-gate-strict` | upsert-gate plus strict no-heap verification (source, symbols, harness)                                                |

### Under The Hood

The `make` targets drive the existing CMake test harness. The first configure creates the local `build/` directory and
fetches Catch2 automatically through CMake `FetchContent`. The build path always uses `cmake --build ... --parallel`,
and tests always run with `ctest --parallel`.

### What Belongs In Git

Check in:

- hand-authored files in `src/`, `tests/`, `specs/`, and project documentation
- build configuration files such as `CMakeLists.txt`

Do not check in:

- `build/` or other CMake build directories
- fetched third-party content under CMake `_deps/`
- local test discovery output, compiler objects, or generated binaries

### Current Test Harness Behavior

- The default test run always executes the executable spec-model tests.
- If a production vector header is added later at `src/vector.hpp`, integration coverage can be enabled automatically by the existing test harness without changing the bootstrap steps.

## cljonic Implementation Characteristics

> **Note**
> The performance of the implementation should be acceptable if the **average collection size is small**, and the
> **cljonic** library is used for **non-performance-critical code**.

- In order to provide the **cljonic immutable-by-default** feature, and because the **heap** can't be used in the **targeted**
  embedded systems, **cljonic** uses deep copying on write.

- In order to meet the **safe** Desired Developer Experience goal, **cljonic** does bounds-checking on all collection
  accesses.

- In order to meet the **safe** Desired Developer Experience goal, **cljonic** provides a **#define** named
  **CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT** that specifies the maximum number of elements that can be stored in a
  **cljonic Collection**, which is defaulted to **1000**. **CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT** can be changed to
  another value if needed by adding a line of code like this `#define CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT 10000`
  before including **cljonic.hpp**, by directly defining **CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT** on the compiler
  command line, or by indirectly defining **CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT** in a build tool like **cmake**.

## Required Resources

- **C++ 23**, or newer, for **Modern C++** features
- **Catch2** v3.7.1, or newer, for testing (fetched automatically by CMake)
- **clang-format** for code formatting
- **clang-tidy** for code analysis
- **doxygen** for code documentation *(coming soon)*
- **gcov** for code coverage analysis
- **genhtml** for code coverage analysis
- **lcov** for code coverage analysis
- **lizard** for code analysis
  - https://github.com/terryyin/lizard
  - According to the lizard documentation, it only supports C++14. Another tool that supports **Cyclomatic Complexity** and **Function LoC** would be better.