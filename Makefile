CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99 -Iinclude
LDFLAGS = -lm

BUILD_DIR = build
SRC_DIR = src
TESTS_DIR = tests

TEST_SRC = $(TESTS_DIR)/test_math.c
MATH_SRC = $(SRC_DIR)/math3d.c
CANVAS_SRC = $(SRC_DIR)/canvas.c

TEST_OBJ = $(BUILD_DIR)/test_math.o
MATH_OBJ = $(BUILD_DIR)/math3d.o
CANVAS_OBJ = $(BUILD_DIR)/canvas.o

TEST_EXEC = $(TESTS_DIR)/test_math2

.PHONY: all clean run

all: $(TEST_EXEC)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_OBJ): $(TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(MATH_OBJ): $(MATH_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(CANVAS_OBJ): $(CANVAS_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_EXEC): $(TEST_OBJ) $(MATH_OBJ) $(CANVAS_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

run: all
	./$(TEST_EXEC)

clean:
	rm -rf $(BUILD_DIR)/*.o $(TEST_EXEC)
