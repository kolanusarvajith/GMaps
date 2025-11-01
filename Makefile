# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Directories and target
SRC_DIR := Phase-1
OBJ_DIR := build
TARGET := phase1

# Source and object files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	@echo "🔗 Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

# Compile step
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🛠️  Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# Ensure build directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up build and binary
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Optional: quick run with test files
run: $(TARGET)
	./$(TARGET) graph.json queries.json output.json
