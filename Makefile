CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

SRC_DIR = src
BUILD_DIR = build

# sourced for the main application
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# exclude timer_script.c from the main application sources
SOURCES := $(filter-out $(SRC_DIR)/timer_script.c, $(SOURCES))

OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

TARGET = pomodoro
TIMER_SCRIPT = timer_script
DIST_DIR = dist

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

# rule to build timer_script separately and link it to sound & countdown
$(TIMER_SCRIPT): src/timer_script.c $(BUILD_DIR)/countdown.o $(BUILD_DIR)/sound.o
	$(CC) $(CFLAGS) -o $@ $< $(BUILD_DIR)/countdown.o $(BUILD_DIR)/sound.o

# rule to compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TIMER_SCRIPT) $(DIST_DIR)
.PHONY: clean

install: $(TARGET) $(TIMER_SCRIPT)
	@mkdir -p $(DIST_DIR)/sounds
	cp $(TARGET) $(DIST_DIR)/
	cp $(TIMER_SCRIPT) $(DIST_DIR)/
	cp -r sounds/*.mp3 $(DIST_DIR)/sounds/

run: install
	./$(DIST_DIR)/$(TARGET)