CC      = gcc
CFLAGS  = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
LDFLAGS = 
SRC     = src/main.c src/parser.c src/builder.c src/micro_shell.c src/aux_builder.c src/simple_ls.c
OBJ     = $(SRC:.c=.o)
BIN     = minimake

.PHONY: all check clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

check:
	$(CC) $(CFLAGS) -c tests/test_micro_shell.c -o tests/test_micro_shell.o
	$(CC) $(CFLAGS) -c src/micro_shell.c -o src/micro_shell_test.o
	$(CC) $(LDFLAGS) tests/test_micro_shell.o src/micro_shell_test.o -lcriterion -o tests/test_micro_shell
	$(CC) $(CFLAGS) -c tests/test_parser.c -o tests/test_parser.o
	$(CC) $(CFLAGS) -c src/parser.c -o src/parser_test.o
	$(CC) $(LDFLAGS) tests/test_parser.o src/parser_test.o -lcriterion -o tests/test_parser
	$(CC) $(CFLAGS) -c tests/test_builder.c -o tests/test_builder.o
	$(CC) $(LDFLAGS) tests/test_builder.o src/builder.o src/micro_shell.o src/simple_ls.o -lcriterion -o tests/test_builder
	$(CC) $(CFLAGS) -c tests/test_aux_builder.c -o tests/test_aux_builder.o
	$(CC) $(LDFLAGS) tests/test_aux_builder.o src/aux_builder.o src/builder.o src/micro_shell.o src/simple_ls.o -lcriterion -o tests/test_aux_builder
	./tests/test_micro_shell
	./tests/test_parser
	./tests/test_builder
	./tests/test_aux_builder

clean:
	$(RM) $(BIN) $(OBJ) tests/test_micro_shell tests/test_parser tests/test_builder tests/test_aux_builder
	$(RM) tests/*.o src/*_test.o