CMAKE ?= cmake
CTEST ?= ctest
BUILD_DIR ?= build
COVERAGE_BUILD_DIR ?= build-coverage
SANITIZER_BUILD_DIR ?= build-sanitizers
COVERAGE_THRESHOLD ?= 100
BROWSER ?= brave-browser
COVERAGE_FILE ?=
LINT_FILE ?=
UPSERT_COVERAGE_FILE ?= vector.hpp
_COVERAGE_SRC = $(if $(COVERAGE_FILE),$(CURDIR)/src/$(COVERAGE_FILE),$(CURDIR)/src/*)

.DEFAULT_GOAL := help

.PHONY: help all test clean configure coverage coverage-cli sanitizer sanitizer-cli format lint no-heap-src no-heap-symbols no-heap upsert-gate upsert-gate-strict

help:
	@printf '%-12s %s\n' 'help' 'Show available targets'
	@printf '%-12s %s\n' 'all' 'Clean, configure, parallel rebuild, and parallel test run'
	@printf '%-12s %s\n' 'test' 'Incremental parallel rebuild and parallel test run'
	@printf '%-12s %s\n' 'clean' 'Remove generated local build output'
	@printf '%-12s %s\n' 'coverage' 'Build with instrumentation, run tests, enforce $(COVERAGE_THRESHOLD)% line coverage'
	@printf '%-12s %s\n' 'coverage-cli' 'Same as coverage but print lines % to stdout; set COVERAGE_FILE=foo.hpp to narrow scope'
	@printf '%-12s %s\n' 'sanitizer' 'Build with ASan+UBSan and run tests'
	@printf '%-12s %s\n' 'sanitizer-cli' 'Quiet ASan+UBSan run for loops; prints sanitizer:ok on pass'
	@printf '%-12s %s\n' 'format' 'Format all source and test C/C++ files in place with clang-format'
	@printf '%-12s %s\n' 'lint' 'Run clang-format and clang-tidy checks; set LINT_FILE=src/foo.hpp or tests/bar.cpp to narrow scope'
	@printf '%-12s %s\n' 'no-heap-src' 'Fail if src contains common heap-allocation APIs or heap-backed STL containers'
	@printf '%-12s %s\n' 'no-heap-symbols' 'Fail if compiled artifact contains forbidden allocator symbols'
	@printf '%-12s %s\n' 'no-heap' 'Strict no-heap gate: source check, harness build, and binary symbol scan'
	@printf '%-12s %s\n' 'upsert-gate' 'Fail-fast loop gate: lint, asan-ubsan, coverage-cli for UPSERT_COVERAGE_FILE'
	@printf '%-12s %s\n' 'upsert-gate-strict' 'upsert-gate plus strict no-heap verification (source, symbols, harness)'

all: clean test

test: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure --parallel

configure: $(BUILD_DIR)/CMakeCache.txt

$(BUILD_DIR)/CMakeCache.txt: CMakeLists.txt
	$(CMAKE) -S . -B $(BUILD_DIR)

coverage:
	$(CMAKE) -S . -B $(COVERAGE_BUILD_DIR) -DCLJONIC_COVERAGE=ON
	$(CMAKE) --build $(COVERAGE_BUILD_DIR) --parallel
	lcov --zerocounters --directory $(COVERAGE_BUILD_DIR)
	$(CTEST) --test-dir $(COVERAGE_BUILD_DIR) --output-on-failure
	lcov --capture --directory $(COVERAGE_BUILD_DIR) \
	     --output-file $(COVERAGE_BUILD_DIR)/coverage.info
	lcov --extract $(COVERAGE_BUILD_DIR)/coverage.info '$(CURDIR)/src/*' \
	     --output-file $(COVERAGE_BUILD_DIR)/coverage-src.info
	genhtml $(COVERAGE_BUILD_DIR)/coverage-src.info \
	        --output-directory $(COVERAGE_BUILD_DIR)/coverage-report
	$(BROWSER) file://$(CURDIR)/$(COVERAGE_BUILD_DIR)/coverage-report/index.html &
	lcov --summary $(COVERAGE_BUILD_DIR)/coverage-src.info \
	     --fail-under-lines $(COVERAGE_THRESHOLD)

coverage-cli:
	@$(CMAKE) -S . -B $(COVERAGE_BUILD_DIR) -DCLJONIC_COVERAGE=ON > /dev/null
	@$(CMAKE) --build $(COVERAGE_BUILD_DIR) --parallel > /dev/null
	@lcov --zerocounters --directory $(COVERAGE_BUILD_DIR) > /dev/null 2>&1
	@$(CTEST) --test-dir $(COVERAGE_BUILD_DIR) --output-on-failure \
	     > $(COVERAGE_BUILD_DIR)/.ctest-out.tmp 2>&1 || \
	     (cat $(COVERAGE_BUILD_DIR)/.ctest-out.tmp; exit 1)
	@lcov --capture --directory $(COVERAGE_BUILD_DIR) \
	     --output-file $(COVERAGE_BUILD_DIR)/coverage.info > /dev/null 2>&1
	@lcov --extract $(COVERAGE_BUILD_DIR)/coverage.info '$(_COVERAGE_SRC)' \
	     --output-file $(COVERAGE_BUILD_DIR)/coverage-src.info > /dev/null 2>&1
	@lcov --summary $(COVERAGE_BUILD_DIR)/coverage-src.info 2>&1 | \
	     awk '/lines\.\.\./{print $$2}'
	@lcov --summary $(COVERAGE_BUILD_DIR)/coverage-src.info \
	     --fail-under-lines $(COVERAGE_THRESHOLD) > /dev/null 2>&1

sanitizer:
	$(CMAKE) -S . -B $(SANITIZER_BUILD_DIR) -DCLJONIC_SANITIZERS=asan-ubsan
	$(CMAKE) --build $(SANITIZER_BUILD_DIR) --parallel
	$(CTEST) --test-dir $(SANITIZER_BUILD_DIR) --output-on-failure --parallel

sanitizer-cli:
	@$(CMAKE) -S . -B $(SANITIZER_BUILD_DIR) -DCLJONIC_SANITIZERS=asan-ubsan > /dev/null
	@$(CMAKE) --build $(SANITIZER_BUILD_DIR) --parallel > /dev/null
	@$(CTEST) --test-dir $(SANITIZER_BUILD_DIR) --output-on-failure \
	     > $(SANITIZER_BUILD_DIR)/.ctest-out.tmp 2>&1 || \
	     (cat $(SANITIZER_BUILD_DIR)/.ctest-out.tmp; exit 1)
	@echo "sanitizer:ok"

format:
	@command -v clang-format > /dev/null 2>&1 || (echo "missing required tool: clang-format" >&2; exit 1)
	@find src tests -type f \( -name '*.hpp' -o -name '*.h' -o -name '*.cpp' -o -name '*.cc' \) -print0 | \
		xargs -0 -r clang-format -i
	@echo "format:ok"

lint: configure
	@$(MAKE) format > /dev/null
	@command -v clang-format > /dev/null 2>&1 || (echo "missing required tool: clang-format" >&2; exit 1)
	@command -v clang-tidy > /dev/null 2>&1 || (echo "missing required tool: clang-tidy" >&2; exit 1)
	@$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON > /dev/null
	@if [ -n "$(LINT_FILE)" ]; then \
		clang-format --dry-run --Werror "$(LINT_FILE)"; \
	else \
		find src tests -type f \( -name '*.hpp' -o -name '*.h' -o -name '*.cpp' -o -name '*.cc' \) -print0 | \
		xargs -0 -r clang-format --dry-run --Werror; \
	fi
	@if [ -n "$(LINT_FILE)" ]; then \
		case "$(LINT_FILE)" in \
			src/*.hpp|src/*.h) clang-tidy tests/vector_spec_tests.cpp -p $(BUILD_DIR) --quiet --header-filter="^$(CURDIR)/(src|tests)/.*" > $(BUILD_DIR)/.clang-tidy-out.tmp 2>&1 ;; \
			*) clang-tidy "$(LINT_FILE)" -p $(BUILD_DIR) --quiet --header-filter="^$(CURDIR)/(src|tests)/.*" > $(BUILD_DIR)/.clang-tidy-out.tmp 2>&1 ;; \
		esac; \
		status=$$?; \
		grep -E "^$(CURDIR)/(src|tests)/|^src/|^tests/" $(BUILD_DIR)/.clang-tidy-out.tmp | grep -Ev ': note:' || true; \
		rm -f $(BUILD_DIR)/.clang-tidy-out.tmp; \
		exit $$status; \
	else \
		clang-tidy tests/vector_spec_tests.cpp -p $(BUILD_DIR) --quiet --header-filter="^$(CURDIR)/(src|tests)/.*" > $(BUILD_DIR)/.clang-tidy-out.tmp 2>&1; \
		status=$$?; \
		grep -E "^$(CURDIR)/(src|tests)/|^src/|^tests/" $(BUILD_DIR)/.clang-tidy-out.tmp | grep -Ev ': note:' || true; \
		rm -f $(BUILD_DIR)/.clang-tidy-out.tmp; \
		exit $$status; \
	fi
	@echo "lint:ok"

no-heap-src:
	@command -v rg > /dev/null 2>&1 || (echo "missing required tool: rg" >&2; exit 1)
	@# Source-level no-heap gate for strict profiles: block common dynamic allocation paths.
	@if rg -n --color never -S -g 'src/**' \
		-e '(^|[^[:alnum:]_])new[[:space:]]' \
		-e '(^|[^[:alnum:]_])delete([[:space:]]|$$)' \
		-e '(^|[^[:alnum:]_])(malloc|calloc|realloc|free|aligned_alloc|posix_memalign)\s*\(' \
		-e 'std::make_unique\s*\(' \
		-e 'std::make_shared\s*\(' \
		-e 'std::allocator\b' \
		-e 'std::pmr::' \
		-e 'std::(vector|deque|list|forward_list|map|multimap|set|multiset|unordered_map|unordered_set|string|basic_string)\b' src; then \
		echo "no-heap-src:fail" >&2; \
		echo "no-heap-src: forbidden heap-allocation pattern found in src" >&2; \
		exit 1; \
	fi
	@echo "no-heap-src:ok"

no-heap-symbols:
	@command -v nm > /dev/null 2>&1 || (echo "missing required tool: nm" >&2; exit 1)
	@# Binary-level no-heap gate: verify no allocator symbols in compiled artifact.
	@BINARY=$(BUILD_DIR)/cljonic_no_heap_probe; \
	if nm "$$BINARY" 2>/dev/null | grep -E '^[0-9a-f]+ [UT] (malloc|free|calloc|realloc|aligned_alloc|posix_memalign|_Znwm|_Znam|_ZdlPv|_ZdaPv)' > /dev/null; then \
		echo "no-heap-symbols:fail" >&2; \
		echo "no-heap-symbols: forbidden allocator symbol found in binary" >&2; \
		exit 1; \
	fi
	@echo "no-heap-symbols:ok"

no-heap:
	@$(MAKE) no-heap-src
	@$(CMAKE) -S . -B $(BUILD_DIR) > /dev/null
	@$(CMAKE) --build $(BUILD_DIR) --target cljonic_no_heap_probe --parallel > /dev/null
	@$(MAKE) no-heap-symbols
	@echo "no-heap:ok"

upsert-gate:
	@$(MAKE) lint
	@$(MAKE) sanitizer-cli
	@$(MAKE) coverage-cli COVERAGE_FILE=$(UPSERT_COVERAGE_FILE)

upsert-gate-strict:
	@$(MAKE) upsert-gate
	@$(MAKE) no-heap

clean:
	rm -rf $(BUILD_DIR) $(COVERAGE_BUILD_DIR) $(SANITIZER_BUILD_DIR) build-missing-vector