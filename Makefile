CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -g
CPPFLAGS := -Iinterpreter -MMD -MP

TARGET := forte

INTERP_DIR := interpreter
MODULE_DIR := modules
BUILD_DIR := build

INTERP_SRCS := $(wildcard $(INTERP_DIR)/*.cpp)
INTERP_OBJS := $(patsubst $(INTERP_DIR)/%.cpp,$(BUILD_DIR)/interpreter/%.o,$(INTERP_SRCS))
INTERP_DEPS := $(INTERP_OBJS:.o=.d)

MODULE_SRCS := $(wildcard $(MODULE_DIR)/*.cpp)
MODULE_OBJS := $(patsubst $(MODULE_DIR)/%.cpp,$(BUILD_DIR)/modules/%.o,$(MODULE_SRCS))
MODULE_DEPS := $(MODULE_OBJS:.o=.d)
MODULE_SOS  := $(patsubst $(MODULE_DIR)/%.cpp,$(MODULE_DIR)/%.so,$(MODULE_SRCS))

all: $(TARGET) modules

$(TARGET): $(INTERP_OBJS)
	$(CXX) $(INTERP_OBJS) -o $@ -rdynamic -ldl

modules: $(MODULE_SOS)

$(BUILD_DIR)/interpreter/%.o: $(INTERP_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/modules/%.o: $(MODULE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC -c $< -o $@

$(MODULE_DIR)/%.so: $(BUILD_DIR)/modules/%.o
	$(CXX) -shared $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
	rm -f $(MODULE_SOS)

re: clean all

-include $(INTERP_DEPS)
-include $(MODULE_DEPS)

.PHONY: all modules run clean re
