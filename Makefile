CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -Ibuild
FLEX := flex
BISON := bison

BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/citu

SRCS := src/ast.cpp src/ast_printer.cpp src/error_reporter.cpp src/main.cpp
OBJS := $(BUILD_DIR)/ast.o $(BUILD_DIR)/ast_printer.o $(BUILD_DIR)/error_reporter.o $(BUILD_DIR)/main.o
GEN_SRCS := $(BUILD_DIR)/citu_parser.cpp $(BUILD_DIR)/citu_lexer.cpp
GEN_OBJS := $(BUILD_DIR)/citu_parser.o $(BUILD_DIR)/citu_lexer.o

all: $(TARGET)

$(TARGET): $(GEN_OBJS) $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/citu_parser.cpp $(BUILD_DIR)/citu_parser.hpp: src/citu_parser.y | $(BUILD_DIR)
	$(BISON) -d -o $(BUILD_DIR)/citu_parser.cpp src/citu_parser.y

$(BUILD_DIR)/citu_lexer.cpp: src/citu_lexer.l $(BUILD_DIR)/citu_parser.hpp | $(BUILD_DIR)
	$(FLEX) -o $(BUILD_DIR)/citu_lexer.cpp src/citu_lexer.l

$(BUILD_DIR)/citu_parser.o: $(BUILD_DIR)/citu_parser.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/citu_lexer.o: $(BUILD_DIR)/citu_lexer.cpp $(BUILD_DIR)/citu_parser.hpp
	$(CXX) $(CXXFLAGS) -Wno-unused-function -c $< -o $@

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

test: $(TARGET)
	python3 tests/run_tests.py

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean test
