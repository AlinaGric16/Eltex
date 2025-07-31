CC := gcc
CFLAGS := -Wall -Wextra -Werror -g

SRC := add.c sub.c mul.c div.c
OBJ = $(SRC:.c=.o)
LIB := libcalc.a
BIN := calculator
HDR := calc.h

all: $(BIN)

%.o: %.c
		$(CC) $(CFLAGS) -c $^

$(LIB): $(OBJ)
		ar rc $@ $^

$(BIN): $(LIB) main.c
		$(CC) $(CFLAGS) main.c -o $@ -L. -lcalc

$(BIN): $(HDR)

clean:
		rm -rf *.o $(LIB) $(BIN)