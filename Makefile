CMAKE ?= cmake
CTEST ?= ctest
BUILD_DIR ?= build

.PHONY: all test clean configure

all: clean test

test: configure
	$(CMAKE) --build $(BUILD_DIR)
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

configure: $(BUILD_DIR)/CMakeCache.txt

$(BUILD_DIR)/CMakeCache.txt: CMakeLists.txt
	$(CMAKE) -S . -B $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) build-missing-vector