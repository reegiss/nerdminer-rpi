# Makefile for NerdMiner Raspberry Pi

TARGET      := nerdminer-rpi
BUILD_DIR   := build
SRC_DIR     := src
INCLUDE_DIR := include

CXX         := g++
CXXFLAGS    := -Wall -Wextra -std=c++17 \
                -I$(INCLUDE_DIR) \
                -MMD -MP
LDFLAGS     := -lpthread -lm

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/**/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(BUILD_DIR) $(OBJS) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Create necessary subdirectories under build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Include auto-generated dependency files
-include $(OBJS:.o=.d)

.PHONY: all clean run