
BIN = lnvis
PREFIX ?= /usr/local
CFLAGS = -ggdb -Os -Wall -Werror -Wextra -std=c99

LDFLAGS = -lSDL2 -lGL


SRCS = main.c
SRCS += $(wildcard deps/*/*.c)

OBJS = $(SRCS:.c=.o)

all: $(BIN)

include $(OBJS:.o=.d)

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

format: fake
	clang-format -i *.c

clean: fake
	rm -f $(OBJS) *.d

.PHONY: fake
