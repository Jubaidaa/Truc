CC      = gcc
CFLAGS  = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
SRC     = src/main.c src/parser.c src/builder.c src/micro_shell.c
BIN     = minimake

.PHONY: all check clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

check:
	$(CC) $(CFLAGS) tests/test_micro_shell.c src/micro_shell.c -lcriterion -o tests/test_micro_shell
	$(CC) $(CFLAGS) tests/test_parser.c src/parser.c -lcriterion -o tests/test_parser
	./tests/test_micro_shell
	./tests/test_parser

clean:
	$(RM) $(BIN) tests/test_micro_shell tests/test_parser rule.txt

