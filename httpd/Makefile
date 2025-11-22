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

.PHONY: httpd check clean

httpd: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $(TARGET)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

check: httpd
	@chmod +x $(TEST_DIR)/response_parser || true
	@patchelf --set-interpreter /lib64/ld-linux-x86-64.so.2 $(TEST_DIR)/response_parser 2>/dev/null || echo "Note: patchelf non installe ou non necessaire"
	$(CC) $(CFLAGS) -o $(TEST_DIR)/test_logger \
		$(TEST_DIR)/test_logger.c \
		src/logger/logger.c \
		src/config/config.c \
		src/utils/string.c
	@$(TEST_DIR)/test_logger
	@python3 -m pytest $(TEST_DIR)

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)
	rm -f $(TEST_DIR)/response_parser
	rm -f config_reader.sh
	rm -f littlegame.tar.gz
	rm -rf test_game
	rm -rf $(TEST_DIR)/www
	rm -f $(TEST_DIR)/*.log
	rm -f $(TEST_DIR)/*.pid
	rm -f $(TEST_DIR)/*.tmp
	rm -f $(TEST_DIR)/test_logger
	rm -rf $(TEST_DIR)/__pycache__
	rm -rf $(TEST_DIR)/.pytest_cache
