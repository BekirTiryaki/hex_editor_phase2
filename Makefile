CC=clang
CFLAGS=-std=c11 -Wall -Wextra -Wpedantic -Werror -O2
SRC=$(wildcard src/*.c)
BIN=bin/hexed

all: $(BIN)

$(BIN): $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

clean:
	rm -rf bin

run: all
	$(BIN) $(FILE)

