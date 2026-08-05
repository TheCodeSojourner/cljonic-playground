CMAKE ?= cmake
CTEST ?= ctest
BUILD_DIR ?= build
COVERAGE_BUILD_DIR ?= build-coverage
COVERAGE_THRESHOLD ?= 100
BROWSER ?= brave-browser

.DEFAULT_GOAL := help

.PHONY: help all test clean configure coverage

help:
	@printf '%-12s %s\n' 'help' 'Show available targets'
	@printf '%-12s %s\n' 'all' 'Clean, configure, parallel rebuild, and parallel test run'
	@printf '%-12s %s\n' 'test' 'Incremental parallel rebuild and parallel test run'
	@printf '%-12s %s\n' 'clean' 'Remove generated local build output'
	@printf '%-12s %s\n' 'coverage' 'Build with instrumentation, run tests, enforce $(COVERAGE_THRESHOLD)% line coverage'

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

clean:
	rm -rf $(BUILD_DIR) $(COVERAGE_BUILD_DIR) build-missing-vector