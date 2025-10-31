CC      = gcc
CFLAGS  = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
SRC     = main.c parser.c builder.c micro_shell.c aux_builder.c simple_ls.c
BIN     = minimake

.PHONY: all check clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

check:
	$(CC) $(CFLAGS) test_micro_shell.c micro_shell.c -lcriterion -o test_micro_shell
	$(CC) $(CFLAGS) test_parser.c parser.c -lcriterion -o test_parser
	$(CC) $(CFLAGS) test_builder.c builder.c micro_shell.c simple_ls.c -lcriterion -o test_builder
	$(CC) $(CFLAGS) test_aux_builder.c aux_builder.c -lcriterion -o test_aux_builder
	./test_micro_shell
	./test_parser
	./test_builder
	./test_aux_builder

clean:
	$(RM) $(BIN) test_micro_shell test_parser test_builder test_aux_builder rule.txt test_rule.txt
