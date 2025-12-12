CC = gcc

# Flags communs
CFLAGS_COMMON = -Wall -Wextra
CFLAGS_OPT    = -O2
CFLAGS_DEBUG  = -O0 -g
CFLAGS_ASAN   = -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer

# Day1 : C standard strict
CFLAGS_DAY1 = $(CFLAGS_COMMON) -pedantic

# Day2 : utilise __int128 (extension GCC)
CFLAGS_DAY2 = $(CFLAGS_COMMON)

BIN1 = day1
BIN2 = day2

SRC1 = day1.c
SRC2 = day2.c

INPUT1 = 01.input
INPUT2 = 02.input

.PHONY: all clean run1 run2 test1 test2 debug asan valgrind1 valgrind2 perf1 perf2

# =====================
# Compilation standard
# =====================
all: $(BIN1) $(BIN2)

$(BIN1): $(SRC1)
	$(CC) $(CFLAGS_OPT) $(CFLAGS_DAY1) -o $@ $<

$(BIN2): $(SRC2)
	$(CC) $(CFLAGS_OPT) $(CFLAGS_DAY2) -o $@ $<

# =========
# Exécution
# =========
run1: $(BIN1)
	./$(BIN1) < $(INPUT1)

run2: $(BIN2)
	@if [ -f $(INPUT2) ]; then \
		./$(BIN2) < $(INPUT2); \
	else \
		echo "Erreur : fichier $(INPUT2) introuvable"; \
	fi

# =====
# Tests
# =====
test1: $(BIN1)
	printf "L68\nL30\nR48\nL5\nR60\nL55\nL1\nL99\n" | ./$(BIN1)

test2: $(BIN2)
	echo "11-22" | ./$(BIN2)

# =================
# Build debug (gdb)
# =================
debug: clean
	$(CC) $(CFLAGS_DEBUG) $(CFLAGS_DAY1) -o $(BIN1) $(SRC1)
	$(CC) $(CFLAGS_DEBUG) $(CFLAGS_DAY2) -o $(BIN2) $(SRC2)

# ====================
# AddressSanitizer
# ====================
asan: clean
	$(CC) $(CFLAGS_ASAN) $(CFLAGS_DAY1) -o $(BIN1) $(SRC1)
	$(CC) $(CFLAGS_ASAN) $(CFLAGS_DAY2) -o $(BIN2) $(SRC2)

# ==========
# Valgrind
# ==========
valgrind1:
	$(MAKE) debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN1) < $(INPUT1)

valgrind2:
	@if [ -f $(INPUT2) ]; then \
		$(MAKE) debug && \
		valgrind --leak-check=full --show-leak-kinds=all ./$(BIN2) < $(INPUT2); \
	else \
		echo "Erreur : fichier $(INPUT2) introuvable"; \
	fi

# =====
# perf
# =====
perf1: $(BIN1)
	perf stat ./$(BIN1) < $(INPUT1)

perf2: $(BIN2)
	@if [ -f $(INPUT2) ]; then \
		perf stat ./$(BIN2) < $(INPUT2); \
	else \
		echo "Erreur : fichier $(INPUT2) introuvable"; \
	fi

# =====
# Clean
# =====
clean:
	rm -f $(BIN1) $(BIN2)
