CC = gcc
CFLAGS = -Wall -Wextra -Werror
LIBS = -lncurses
SRC = src/main.c src/panel.c src/ui.c
OBJ = $(SRC:.c=.o)
BIN = mc

all: $(BIN)

run: $(BIN)
	./$(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean