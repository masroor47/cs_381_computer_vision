# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I./headers

# Directory settings
SRC_DIR = src
HEADER_DIR = headers
OBJ_DIR = obj
BIN_DIR = bin

# Find all source files and generate object file names
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Main target name
TARGET = $(BIN_DIR)/main

# Default target
all: dirs $(TARGET)

# Create necessary directories
dirs:
	mkdir -p $(SRC_DIR) $(HEADER_DIR) $(OBJ_DIR) $(BIN_DIR)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*

# Clean and rebuild
rebuild: clean all

.PHONY: all dirs clean rebuild
