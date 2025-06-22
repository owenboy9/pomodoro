CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC_DIR = SRC_DIRBUILD_DIR = build
INCLUDE_DIR = INCLUDE_DIR
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

TARGET = pomodoro

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -P $(BUILD_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
.PHONY: clean