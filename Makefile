CMAKE ?= cmake
CTEST ?= ctest
BUILD_DIR ?= build

.DEFAULT_GOAL := help

.PHONY: help all test clean configure

help:
	@printf '%-12s %s\n' 'help' 'Show available targets'
	@printf '%-12s %s\n' 'all' 'Clean, configure, rebuild, and run tests'
	@printf '%-12s %s\n' 'test' 'Incrementally rebuild what changed and run tests'
	@printf '%-12s %s\n' 'clean' 'Remove generated local build output'

all: clean test

test: configure
	$(CMAKE) --build $(BUILD_DIR)
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

configure: $(BUILD_DIR)/CMakeCache.txt

$(BUILD_DIR)/CMakeCache.txt: CMakeLists.txt
	$(CMAKE) -S . -B $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) build-missing-vector