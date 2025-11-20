CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -g -D_POSIX_C_SOURCE=200809L
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
TARGET = httpd

CONFIG_DIR = $(SRC_DIR)/config
DAEMON_DIR = $(SRC_DIR)/daemon
HTTP_DIR = $(SRC_DIR)/http
LOGGER_DIR = $(SRC_DIR)/logger
SERVER_DIR = $(SRC_DIR)/server
STRING_DIR = $(SRC_DIR)/utils/string

CONFIG_SRC = $(CONFIG_DIR)/config.c
CONFIG_OBJ = $(BUILD_DIR)/config.o

DAEMON_SRC = $(DAEMON_DIR)/daemon.c
DAEMON_OBJ = $(BUILD_DIR)/daemon.o

HTTP_SRC = $(HTTP_DIR)/http.c
HTTP_OBJ = $(BUILD_DIR)/http.o

LOGGER_SRC = $(LOGGER_DIR)/logger.c
LOGGER_OBJ = $(BUILD_DIR)/logger.o

SERVER_SRC = $(SERVER_DIR)/server.c
SERVER_OBJ = $(BUILD_DIR)/server.o

STRING_SRC = $(STRING_DIR)/string.c
STRING_OBJ = $(BUILD_DIR)/string.o

MAIN_SRC = $(SRC_DIR)/main.c
MAIN_OBJ = $(BUILD_DIR)/main.o

ALL_OBJS = $(CONFIG_OBJ) $(DAEMON_OBJ) $(HTTP_OBJ) $(LOGGER_OBJ) $(SERVER_OBJ) $(STRING_OBJ) $(MAIN_OBJ)

.PHONY: httpd check clean

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CONFIG_OBJ): $(CONFIG_SRC) $(CONFIG_DIR)/config.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DAEMON_OBJ): $(DAEMON_SRC) $(DAEMON_DIR)/daemon.h $(CONFIG_DIR)/config.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(HTTP_OBJ): $(HTTP_SRC) $(HTTP_DIR)/http.h $(STRING_DIR)/string.h $(STRING_DIR)/aux_string.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LOGGER_OBJ): $(LOGGER_SRC) $(LOGGER_DIR)/logger.h $(CONFIG_DIR)/config.h $(HTTP_DIR)/http.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_OBJ): $(SERVER_SRC) $(SERVER_DIR)/server.h $(CONFIG_DIR)/config.h $(HTTP_DIR)/http.h $(LOGGER_DIR)/logger.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(STRING_OBJ): $(STRING_SRC) $(STRING_DIR)/string.h $(STRING_DIR)/aux_string.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC) $(CONFIG_DIR)/config.h $(DAEMON_DIR)/daemon.h $(SERVER_DIR)/server.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

httpd: $(TARGET)

$(TARGET): $(ALL_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build successful: ./$(TARGET)"

check: $(TARGET)
	@echo "Running basic checks..."
	@./$(TARGET) --help || true
	@echo "Build complete. Manual testing required."

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
	@echo "Clean complete"
