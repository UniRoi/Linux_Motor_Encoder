# Variables
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude -MMD -MP  # Include your library's headers
LDFLAGS := -lpthread -lgpiod #-lpigpio#-Llib -l:fifo.a  # Link pthread
SRC_DIR := src
OBJ_DIR := obj
TARGET := speedCtrl

# Source and object files
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Dependency files
DEPS := $(OBJ:%.o=%.d)

# didnt work with the library
# Custom library source and object files
# LIB_SRC := $(wildcard lib/fifo/*.cpp)
# LIB_OBJ := $(LIB_SRC:%.cpp=%.o)
# LIB_TARGET := lib/fifo.a  # Static library location

# Default target
all: $(TARGET) #$(LIB_TARGET)

# Rule to link object files and create the executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Rule to build the static library
# $(LIB_TARGET): $(LIB_OBJ)
# 	ar rcs $@ $(LIB_OBJ)  # Create the static library (libmylib.a) from object files

# Rule to compile .cpp files to .o files, also generating .d files for dependencies
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile library .cpp files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Include dependency files, if they exist
-include $(DEPS)

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(LIB_OBJ) $(LIB_TARGET)

# Phony targets
.PHONY: all clean
