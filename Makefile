
BIN = lnvis
PREFIX ?= /usr/local
CFLAGS = -ggdb -Os -Wall -Werror -Wextra -std=c99

LDFLAGS = -lSDL -lGL

DEPS = main.o

include $(OBJS:.o=.d)

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

all: $(BIN)

$(BIN): main.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

format: fake
	clang-format -i *.c

clean: fake
	rm -f $(DEPS)

.PHONY: fake
