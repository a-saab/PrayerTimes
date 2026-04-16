CXX = g++
CXXFLAGS = -std=c++17 -Isrc -Wall -Wextra -O2

SRC = src/PrayerTimes.cpp
TESTS = \
	test/test_adjustments.cpp \
	test/test_high_latitude.cpp \
	test/test_invariants.cpp \
	test/test_methods.cpp \
	test/test_regression.cpp \
	test/test_time_utils.cpp \
	test/test_validation.cpp

OUT_DIR = build
BINS = $(TESTS:%.cpp=$(OUT_DIR)/%)

.PHONY: all build-tests test clean

all: test

$(OUT_DIR):
	mkdir -p $(OUT_DIR)/test

# Build each test as its own binary
$(OUT_DIR)/test/%: test/%.cpp $(SRC) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $@

# Build all test binaries
build-tests: $(BINS)

# Run all tests
test: build-tests
	@fail=0; \
	for t in $(BINS); do \
		echo "Running $$t"; \
		./$$t || fail=1; \
	done; \
	if [ $$fail -eq 0 ]; then \
		echo "All tests passed."; \
	else \
		echo "Some tests failed."; \
		exit 1; \
	fi

# Clean
clean:
	rm -rf $(OUT_DIR)
