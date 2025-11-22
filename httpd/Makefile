CC      = gcc

SRC_DIR   = src
BUILD_DIR = build
TARGET    = httpd
TEST_DIR  = tests

# Sources
SRCS = \
    src/main.c \
    src/config/config.c \
    src/daemon/daemon.c \
    src/http/http.c \
    src/logger/logger.c \
    src/server/aux_network.c \
    src/server/server.c \
    src/utils/string.c

OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:src/%=$(BUILD_DIR)/%)

# Flags (Do not modify)
CFLAGS  = -Wall -Wextra -Werror -std=c99 -pedantic -g -D_POSIX_C_SOURCE=200809L
CPPFLAGS =
LDFLAGS =

.PHONY: httpd check clean

# Main target (first one runs by default)
httpd: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $(TARGET)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

check: httpd
	@echo "--- Setting up tests ---"
	@chmod +x $(TEST_DIR)/response_parser || true
	@echo "--- Building C Unit Tests ---"
	$(CC) $(CFLAGS) -o $(TEST_DIR)/test_logger \
		$(TEST_DIR)/test_logger.c \
		src/logger/logger.c \
		src/config/config.c \
		src/utils/string.c
	@echo "--- Running C Unit Tests ---"
	@$(TEST_DIR)/test_logger
	@echo "--- Running Python Functional & Compliance Tests ---"
	@python3 -m pytest $(TEST_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
	rm -f $(TEST_DIR)/test_logger
	rm -rf $(TEST_DIR)/__pycache__
	rm -rf $(TEST_DIR)/.pytest_cache
	rm -rf $(TEST_DIR)/www
	rm -f $(TEST_DIR)/*.log
	rm -f $(TEST_DIR)/*.pid
	rm -f $(TEST_DIR)/*.tmp
