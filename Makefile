
BIN = lnvis
PREFIX ?= /usr/local
CFLAGS = -Ideps -ggdb -Os -Wall -Werror -Wextra -std=c99 \
         -Wno-implicit-fallthrough

LDFLAGS = -lglfw -lGL


SRCS = main.c

SRCS += update.c
SRCS += render.c
SRCS += perf.c
SRCS += demo.c
SRCS += $(wildcard deps/*/*.c)

OBJS = $(SRCS:.c=.o)

all: $(BIN)

include $(OBJS:.o=.d)

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

TAGS: fake
	etags $(SRCS)

format: fake
	clang-format -i *.c

clean: fake
	rm -f $(OBJS) *.d

.PHONY: fake
