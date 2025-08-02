CC := gcc
CFLAGS := -Wall -Wextra -Werror -g

SRC := add.c sub.c mul.c div.c
OBJ = $(SRC:.c=.o)
LIB := libcalc.so
BIN := calculator
HDR := calc.h

all: $(LIB)

%.o: %.c
		$(CC) $(CFLAGS) $^ -fPIC -c

$(LIB): $(OBJ)
		$(CC) -shared $^ -o $@

$(BIN): $(LIB) main.c
		$(CC) $(CFLAGS) main.c -o $@ -L. -lcalc

run: $(BIN)
	LD_LIBRARY_PATH=$$PWD:$$LD_LIBRARY_PATH ./$(BIN)

$(BIN): $(HDR)


$(BIN)_dl: $(LIB) main_dl.c
		$(CC) $(CFLAGS) main_dl.c $(LIB) -o $@ -ldl -Wl,-rpath,.

run_dl: $(BIN)_dl
	./$(BIN)_dl

clean:
		rm -rf *.o $(LIB) $(BIN) $(BIN)_dl