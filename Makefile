CMAKE ?= cmake
CTEST ?= ctest
BUILD_DIR ?= build
COVERAGE_BUILD_DIR ?= build-coverage
COVERAGE_THRESHOLD ?= 100
BROWSER ?= brave-browser
COVERAGE_FILE ?=
_COVERAGE_SRC = $(if $(COVERAGE_FILE),$(CURDIR)/src/$(COVERAGE_FILE),$(CURDIR)/src/*)

.DEFAULT_GOAL := help

.PHONY: help all test clean configure coverage coverage-cli

help:
	@printf '%-12s %s\n' 'help' 'Show available targets'
	@printf '%-12s %s\n' 'all' 'Clean, configure, parallel rebuild, and parallel test run'
	@printf '%-12s %s\n' 'test' 'Incremental parallel rebuild and parallel test run'
	@printf '%-12s %s\n' 'clean' 'Remove generated local build output'
	@printf '%-12s %s\n' 'coverage' 'Build with instrumentation, run tests, enforce $(COVERAGE_THRESHOLD)% line coverage'
	@printf '%-12s %s\n' 'coverage-cli' 'Same as coverage but print lines % to stdout; set COVERAGE_FILE=foo.hpp to narrow scope'

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

clean:
	rm -rf $(BUILD_DIR) $(COVERAGE_BUILD_DIR) build-missing-vector