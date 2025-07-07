# Compiler and settings
CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c99
LDFLAGS = -lm

# Source files
CANVAS_SRC = src/canvas.c
MATH_SRC = src/math3d.c
RENDER_SRC = src/renderer.c
LIGHTING_SRC = src/lighting.c

# Demo/test files
CLOCK_DEMO = demo/main.c
MATH_TEST = tests/test_math.c
RENDER_DEMO = demo/soccer_demo.c
LIGHTING_DEMO = demo/lighting_demo.c

# Output directories and files
BUILD_DIR = build
CLOCK_OUT = $(BUILD_DIR)/clock_demo
MATH_OUT = $(BUILD_DIR)/test_math
RENDER_OUT = $(BUILD_DIR)/render_demo
LIGHTING_OUT = $(BUILD_DIR)/lighting_demo

# Phony targets
.PHONY: all clean run_clock run_math run_render run_lighting

# Default target
all: $(CLOCK_OUT) $(MATH_OUT) $(RENDER_OUT) $(LIGHTING_OUT)

# Create build directory
$(BUILD_DIR):
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"

# Individual build targets
$(CLOCK_OUT): $(CANVAS_SRC) $(CLOCK_DEMO) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(MATH_OUT): $(MATH_SRC) $(CANVAS_SRC) $(MATH_TEST) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(RENDER_OUT): $(CANVAS_SRC) $(MATH_SRC) $(RENDER_SRC) $(RENDER_DEMO) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(LIGHTING_OUT): $(CANVAS_SRC) $(MATH_SRC) $(RENDER_SRC) $(LIGHTING_SRC) $(LIGHTING_DEMO) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Run targets
run_clock: $(CLOCK_OUT)
	@$(CLOCK_OUT)

run_cube: $(MATH_OUT)
	@$(MATH_OUT)

run_render: $(RENDER_OUT)
	@$(RENDER_OUT)

run_lighting: $(LIGHTING_OUT)
	@$(LIGHTING_OUT)

# Clean everything
clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
	@del /q *.ppm *.pgm *.exe 2>nul
	@echo Cleaned all build artifacts