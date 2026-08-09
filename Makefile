CMAKE ?= cmake
CTEST ?= ctest
BUILD_DIR ?= build
COVERAGE_BUILD_DIR ?= build-coverage
SANITIZER_BUILD_DIR ?= build-sanitizers
CYCLOMATIC_COMPLEXITY_THRESHOLD ?= 4
FUNCTION_LENGTH_THRESHOLD ?= 25
COMPLEXITY_PATH ?= src
COVERAGE_THRESHOLD ?= 100
BROWSER ?= brave-browser
COVERAGE_FILE ?=
LINT_FILE ?=
UPSERT_COVERAGE_FILE ?= cljonic-vector.hpp
UPSERT_FAST_FILE ?= src/$(UPSERT_COVERAGE_FILE)
UPSERT_FAST_COMPLEXITY_PATH ?= $(UPSERT_FAST_FILE)
_COVERAGE_SRC = $(if $(COVERAGE_FILE),$(CURDIR)/src/$(COVERAGE_FILE),$(CURDIR)/src/*)
TRACEABILITY_DIR ?= spec-to-code-traceability
TRACEABILITY_SNAPSHOT ?= $(TRACEABILITY_DIR)/spec-to-code-obligation-ids.snapshot.txt
TRACEABILITY_IDS_CURRENT ?= $(BUILD_DIR)/.traceability-obligation-ids.current.tmp
TRACEABILITY_TEST_IDS_CURRENT ?= $(BUILD_DIR)/.traceability-ids-in-tests.tmp

.DEFAULT_GOAL := help

.PHONY: help all test clean configure coverage coverage-cli sanitizer sanitizer-cli complexity complexity-cli format format-doc-samples lint no-heap-src no-heap-symbols no-heap _traceability-obligation-ids-current _traceability-test-ids-current traceability-spec-to-code traceability-spec-to-code-update-snapshot traceability-category-report upsert-fast upsert-gate upsert-gate-strict docs

help:
	@printf '%-12s %s\n' 'all' 'Clean, configure, parallel rebuild, and parallel test run'
	@printf '%-12s %s\n' 'clean' 'Remove generated local build output'
	@printf '%-12s %s\n' 'complexity' 'Run lizard on COMPLEXITY_PATH (default src); set CYCLOMATIC_COMPLEXITY_THRESHOLD and FUNCTION_LENGTH_THRESHOLD'
	@printf '%-12s %s\n' 'complexity-cli' 'Quiet lizard warning-only check on COMPLEXITY_PATH; fails if thresholds are exceeded'
	@printf '%-12s %s\n' 'coverage' 'Build with instrumentation, run tests, enforce $(COVERAGE_THRESHOLD)% line coverage'
	@printf '%-12s %s\n' 'coverage-cli' 'Same as coverage but print lines % to stdout; set COVERAGE_FILE=foo.hpp to narrow scope'
	@printf '%-12s %s\n' 'docs' 'Generate Doxygen HTML documentation to docs/'
	@printf '%-12s %s\n' 'format' 'Format source/test C/C++ files and Doxygen C++ sample blocks'
	@printf '%-12s %s\n' 'format-doc-samples' 'Format Doxygen C++ sample blocks in src/* headers with clang-format'
	@printf '%-12s %s\n' 'help' 'Show available targets'
	@printf '%-12s %s\n' 'lint' 'Run clang-format and clang-tidy checks; set LINT_FILE=src/foo.hpp or tests/bar.cpp to narrow scope'
	@printf '%-12s %s\n' 'no-heap' 'Strict no-heap gate: source check, harness build, and binary symbol scan'
	@printf '%-12s %s\n' 'no-heap-src' 'Fail if src contains common heap-allocation APIs or heap-backed STL containers'
	@printf '%-12s %s\n' 'no-heap-symbols' 'Fail if compiled artifact contains forbidden allocator symbols'
	@printf '%-12s %s\n' 'sanitizer' 'Build with ASan+UBSan and run tests'
	@printf '%-12s %s\n' 'sanitizer-cli' 'Quiet ASan+UBSan run for loops; prints sanitizer:ok on pass'
	@printf '%-12s %s\n' 'test' 'Incremental parallel rebuild and parallel test run'
	@printf '%-12s %s\n' 'traceability-category-report' 'Non-blocking obligation-family diagnostics from snapshot vs test TRACE_ID coverage'
	@printf '%-12s %s\n' 'traceability-spec-to-code' 'Strict spec-to-code traceability gate (set-scoped allium, snapshot sync, test macro trace coverage)'
	@printf '%-12s %s\n' 'traceability-spec-to-code-update-snapshot' 'Regenerate committed spec-to-code obligation snapshot from current specs'
	@printf '%-12s %s\n' 'upsert-fast' 'Fast scoped loop: lint and complexity-cli for UPSERT_FAST_FILE'
	@printf '%-12s %s\n' 'upsert-gate' 'Fail-fast loop gate: lint, complexity-cli, asan-ubsan, coverage-cli for UPSERT_COVERAGE_FILE'
	@printf '%-12s %s\n' 'upsert-gate-strict' 'upsert-gate plus strict spec-to-code traceability and no-heap verification'

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
	     awk '/lines\.\.\./{printf "coverage:lines=%s\n", $$2}'
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

complexity:
	@command -v lizard > /dev/null 2>&1 || (echo "missing required tool: lizard" >&2; exit 1)
	@lizard -C $(CYCLOMATIC_COMPLEXITY_THRESHOLD) -L $(FUNCTION_LENGTH_THRESHOLD) $(COMPLEXITY_PATH)

complexity-cli:
	@command -v lizard > /dev/null 2>&1 || (echo "missing required tool: lizard" >&2; exit 1)
	@lizard -C $(CYCLOMATIC_COMPLEXITY_THRESHOLD) -L $(FUNCTION_LENGTH_THRESHOLD) -w $(COMPLEXITY_PATH)
	@echo "complexity:ok"

format:
	@command -v clang-format > /dev/null 2>&1 || (echo "missing required tool: clang-format" >&2; exit 1)
	@find src tests -type f \( -name '*.hpp' -o -name '*.h' -o -name '*.cpp' -o -name '*.cc' \) -print0 | \
		xargs -0 -r clang-format -i
	@$(MAKE) --no-print-directory -s format-doc-samples > /dev/null
	@echo "format:ok"

format-doc-samples:
	@command -v clang-format > /dev/null 2>&1 || (echo "missing required tool: clang-format" >&2; exit 1)
	@find src -type f \( -name '*.hpp' -o -name '*.h' \) -print0 | \
		xargs -0 -r scripts/format-doc-samples.pl
	@echo "format-doc-samples:ok"

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
			src/*.hpp|src/*.h) clang-tidy tests/cljonic-vector-spec-tests.cpp -p $(BUILD_DIR) --quiet --header-filter="^$(CURDIR)/(src|tests)/.*" > $(BUILD_DIR)/.clang-tidy-out.tmp 2>&1 ;; \
			*) clang-tidy "$(LINT_FILE)" -p $(BUILD_DIR) --quiet --header-filter="^$(CURDIR)/(src|tests)/.*" > $(BUILD_DIR)/.clang-tidy-out.tmp 2>&1 ;; \
		esac; \
		status=$$?; \
		grep -E "^$(CURDIR)/(src|tests)/|^src/|^tests/" $(BUILD_DIR)/.clang-tidy-out.tmp | grep -Ev ': note:' || true; \
		rm -f $(BUILD_DIR)/.clang-tidy-out.tmp; \
		exit $$status; \
	else \
		clang-tidy tests/cljonic-vector-spec-tests.cpp -p $(BUILD_DIR) --quiet --header-filter="^$(CURDIR)/(src|tests)/.*" > $(BUILD_DIR)/.clang-tidy-out.tmp 2>&1; \
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
	@$(MAKE) --no-print-directory -s no-heap-src
	@$(CMAKE) -S . -B $(BUILD_DIR) > /dev/null
	@$(CMAKE) --build $(BUILD_DIR) --target cljonic_no_heap_probe --parallel > /dev/null
	@$(MAKE) --no-print-directory -s no-heap-symbols
	@echo "no-heap:ok"

traceability-spec-to-code-update-snapshot:
	@mkdir -p $(TRACEABILITY_DIR)
	@$(MAKE) --no-print-directory -s _traceability-obligation-ids-current
	@cp $(TRACEABILITY_IDS_CURRENT) $(TRACEABILITY_SNAPSHOT)
	@echo "traceability-spec-to-code-update-snapshot:ok"

_traceability-obligation-ids-current:
	@command -v allium > /dev/null 2>&1 || (echo "missing required tool: allium" >&2; exit 1)
	@command -v rg > /dev/null 2>&1 || (echo "missing required tool: rg" >&2; exit 1)
	@mkdir -p $(BUILD_DIR)
	@spec_files=$$(find specs -type f -name '*.allium' | sort); \
	if [ -z "$$spec_files" ]; then \
		echo "traceability: no .allium files under specs/" >&2; \
		exit 1; \
	fi; \
	for f in $$spec_files; do allium plan "$$f"; done | \
		rg -No '"id":\s*"([^"]+)"' | \
		sed -E 's/.*"id":\s*"([^"]+)"/\1/' | \
		sort -u > $(TRACEABILITY_IDS_CURRENT)

_traceability-test-ids-current:
	@command -v perl > /dev/null 2>&1 || (echo "missing required tool: perl" >&2; exit 1)
	@mkdir -p $(BUILD_DIR)
	@find tests -type f \( -name '*.cpp' -o -name '*.cc' \) -print0 | \
		xargs -0 -r cat | \
		sed -E '/^[[:space:]]*#define[[:space:]]+TRACE_ID\(/d' | \
		perl -0777 -ne 'while(/TRACE_ID\((.*?)\);/sg){@s=($$1 =~ /"([^"]*)"/g); print join("", @s), "\n" if @s;}' | \
		sort -u > $(TRACEABILITY_TEST_IDS_CURRENT)

traceability-spec-to-code:
	@test -f $(TRACEABILITY_SNAPSHOT) || (echo "missing snapshot: $(TRACEABILITY_SNAPSHOT). Run 'make traceability-spec-to-code-update-snapshot'." >&2; exit 1)
	@allium check specs > /dev/null
	@allium analyse specs > /dev/null
	@$(MAKE) --no-print-directory -s _traceability-obligation-ids-current
	@diff -u $(TRACEABILITY_SNAPSHOT) $(TRACEABILITY_IDS_CURRENT) > /dev/null || \
		(echo "traceability-spec-to-code: snapshot drift detected. Run 'make traceability-spec-to-code-update-snapshot'." >&2; diff -u $(TRACEABILITY_SNAPSHOT) $(TRACEABILITY_IDS_CURRENT) >&2; exit 1)
	@$(MAKE) --no-print-directory -s _traceability-test-ids-current
	@if [ ! -s $(TRACEABILITY_TEST_IDS_CURRENT) ]; then \
		echo "traceability-spec-to-code: no TRACE_ID references found in tests" >&2; \
		exit 1; \
	fi
	@if ! diff -u $(TRACEABILITY_SNAPSHOT) $(TRACEABILITY_TEST_IDS_CURRENT) > /dev/null; then \
		echo "traceability-spec-to-code: test TRACE_ID coverage does not match snapshot obligations" >&2; \
		diff -u $(TRACEABILITY_SNAPSHOT) $(TRACEABILITY_TEST_IDS_CURRENT) >&2; \
		exit 1; \
	fi
	@for f in $$(find tests -type f \( -name '*.cpp' -o -name '*.cc' \) | sort); do \
		awk 'BEGIN { RS = "TEST_CASE\\("; ORS = "" } \
		NR > 1 { block = "TEST_CASE(" $$0; \
		if (block ~ /TRACE_ID[[:space:]]*\(/) { \
			if (block !~ /CHECK[[:alnum:]_]*[[:space:]]*\(/ && block !~ /REQUIRE[[:alnum:]_]*[[:space:]]*\(/) { \
				print "traceability-spec-to-code: traced block missing assertion in " FILENAME "\\n" > "/dev/stderr"; \
				exit 1; \
			} \
		} \
		}' "$$f" || exit 1; \
	done
	@echo "traceability-spec-to-code:ok"

traceability-category-report:
	@command -v perl > /dev/null 2>&1 || { echo "traceability-category-report:status=skip:reason=missing_perl"; exit 0; }
	@test -f $(TRACEABILITY_SNAPSHOT) || { echo "traceability-category-report:status=skip:reason=missing_snapshot:path=$(TRACEABILITY_SNAPSHOT)"; exit 0; }
	@$(MAKE) --no-print-directory -s _traceability-test-ids-current
	@if [ ! -s $(TRACEABILITY_TEST_IDS_CURRENT) ]; then \
		echo "traceability-category-report:status=warn:reason=no_trace_ids_in_tests"; \
		exit 0; \
	fi
	@awk -F'.' '{print $$1}' $(TRACEABILITY_SNAPSHOT) | sort | uniq -c | awk '{printf "traceability-category-report:category=%s:snapshot=%s\n", $$2, $$1}'
	@awk -F'.' '{print $$1}' $(TRACEABILITY_TEST_IDS_CURRENT) | sort | uniq -c | awk '{printf "traceability-category-report:category=%s:tests=%s\n", $$2, $$1}'
	@comm -23 $(TRACEABILITY_SNAPSHOT) $(TRACEABILITY_TEST_IDS_CURRENT) | \
		awk -F'.' '{c[$$1]++} END {for (k in c) printf "traceability-category-report:category=%s:missing=%s\n", k, c[k]}' | sort
	@comm -13 $(TRACEABILITY_SNAPSHOT) $(TRACEABILITY_TEST_IDS_CURRENT) | \
		awk -F'.' '{c[$$1]++} END {for (k in c) printf "traceability-category-report:category=%s:extra=%s\n", k, c[k]}' | sort
	@echo "traceability-category-report:status=ok"

upsert-fast:
	@$(MAKE) --no-print-directory -s lint LINT_FILE=$(UPSERT_FAST_FILE)
	@$(MAKE) --no-print-directory -s complexity-cli COMPLEXITY_PATH=$(UPSERT_FAST_COMPLEXITY_PATH)
	@echo "upsert-fast:ok:file=$(UPSERT_FAST_FILE)"

upsert-gate:
	@$(MAKE) --no-print-directory -s lint
	@$(MAKE) --no-print-directory -s complexity-cli
	@$(MAKE) --no-print-directory -s sanitizer-cli
	@$(MAKE) --no-print-directory -s coverage-cli COVERAGE_FILE=$(UPSERT_COVERAGE_FILE)

upsert-gate-strict:
	@$(MAKE) --no-print-directory -s upsert-gate
	@$(MAKE) --no-print-directory -s traceability-spec-to-code
	@$(MAKE) --no-print-directory -s no-heap

docs:
	@command -v doxygen > /dev/null 2>&1 || (echo "missing required tool: doxygen" >&2; exit 1)
	@rm -rf docs/
	@cd doc && doxygen Doxyfile > /dev/null
	@echo "docs:ok"

clean:
	rm -rf $(BUILD_DIR) $(COVERAGE_BUILD_DIR) $(SANITIZER_BUILD_DIR) build-missing-vector