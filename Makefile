
BIN = lnvis
PREFIX ?= /usr/local
CFLAGS = -Ideps -ggdb -O2 -Wall -Wextra -std=c99 -DJSMN_PARENT_LINKS $(EXTRA_CFLAGS)

LDFLAGS = $(EXTRA_LDFLAGS) -lglfw -lm

ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CFLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CFLAGS += -D LINUX
	LDFLAGS += -lGL
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -D OSX
	LDFLAGS += -framework OpenGL
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CFLAGS += -D ARM
    endif
endif

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

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

include $(OBJS:.o=.d)


$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS)  -o $@

TAGS: fake
	etags $(SRCS)

format: fake
	clang-format -i *.c

clean: fake
	rm -f $(OBJS) *.d

.PHONY: fake
