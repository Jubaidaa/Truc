# === VARIABLE DEFINITIONS ===
CC       = gcc
CFLAGS   = -std=c99 -pedantic -Wall -Wextra -Wvla
SRC      = src/main.c src/parser.c src/builder.c src/aux_builder.c src/micro_shell.c src/utils.c
OBJ      = $(SRC:.c=.o)
BIN      = minimake
MSG      = Build finished successfully!

# === PHONY TARGETS ===
.PHONY: all clean rebuild showvars test_auto test_vars test_at test_chain check

# === MAIN BUILD ===
all: $(BIN)
	@echo "✅ All build done, test passed!"

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)
	@echo "$(MSG)"

# === CLEAN ===
clean:
	$(RM) $(BIN)
	@echo "🧹 Clean done"

# === REBUILD ===
rebuild: clean all
	@echo "♻️ Rebuild complete"

# === SHOW VARIABLE EXPANSION ===
showvars:
	@echo "Compiler: $(CC)"
	@echo "Flags: $(CFLAGS)"
	@echo "Sources: $(SRC)"
	@echo "Binary: $(BIN)"

# === TEST: AUTOMATIC VARIABLES ($@, $<, $^) ===
test_auto: first second
	@echo "All automatic vars test done!"

first:
	@echo "Target: $@"
	@echo "Deps: $^"
	@echo "First dep: $<"

second: first
	@echo "Target: $@"
	@echo "Depends on: $^"

# === TEST: VARIABLE EXPANSION ===
test_vars:
	@echo "Testing VAR expansions..."
	@echo "Using CC=$(CC) and CFLAGS=$(CFLAGS)"
	@echo "Final binary will be $(BIN)"

# === TEST: COMMAND SUPPRESSION (@) ===
test_at:
	echo "This line will be printed (command + output)"
	@echo "This line will only show output"
	@echo "✅ @ suppression test passed"

# === TEST: CHAINED DEPENDENCIES ===
test_chain: step1

step1: step2
	@echo "step1 done"

step2: step3
	@echo "step2 done"

step3:
	@echo "step3 done"

# === GLOBAL TEST SUITE ===
check: test_at test_vars test_auto test_chain showvars
	@echo ""
	@echo "========================================"
	@echo "✅  All Minimake feature tests completed!"
	@echo "========================================"

