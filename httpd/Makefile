CC      = gcc

SRC_DIR   = src
BUILD_DIR = build
TARGET    = httpd
TEST_DIR  = tests

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

CFLAGS  = -Wall -Wextra -Werror -std=c99 -pedantic -g -D_POSIX_C_SOURCE=200809L
CPPFLAGS =
LDFLAGS =

.PHONY: all httpd check clean fclean re

all: httpd

httpd: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $(TARGET)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

check: httpd
	@echo "Running test suite..."
	@chmod +x $(TEST_DIR)/run_tests.sh
	@$(TEST_DIR)/run_tests.sh

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(TARGET)
	rm -rf $(TEST_DIR)/www
	rm -f $(TEST_DIR)/*.log
	rm -f $(TEST_DIR)/*.pid

re: fclean all
