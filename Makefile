
BIN = lnvis
PREFIX ?= /usr/local
CFLAGS = -Ideps -ggdb -O2 -Wall -Wextra -std=c99 -DJSMN_PARENT_LINKS $(TRAVIS_CFLAGS)

LDFLAGS = $(TRAVIS_LDFLAGS) -lglfw -lGL -lm


SRCS = main.c

SRCS += update.c
SRCS += ln.c
SRCS += grid.c
SRCS += render.c
SRCS += perf.c
SRCS += json.c
SRCS += demo.c
SRCS += $(wildcard deps/*.c)
SRCS += $(wildcard deps/*/*.c)

OBJS = $(SRCS:.c=.o)

all: $(BIN)

include $(OBJS:.o=.d)

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS)  -o $@

TAGS: fake
	etags $(SRCS)

format: fake
	clang-format -i *.c

clean: fake
	rm -f $(OBJS) *.d

.PHONY: fake
