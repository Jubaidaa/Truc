CC      = gcc
CFLAGS  = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
SRC     = main.c parser.c builder.c micro_shell.c rule_parser.c
BIN     = minimake

.PHONY: all check clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

check:
	$(CC) $(CFLAGS) test_micro_shell.c micro_shell.c -lcriterion -o test_micro_shell
	$(CC) $(CFLAGS) test_parser.c parser.c -lcriterion -o test_parser
	./test_micro_shell
	./test_parser

clean:
	$(RM) $(BIN) test_micro_shell test_parser rule.txt
