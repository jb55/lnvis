
#ifndef LNVIS_DEFS_H
#define LNVIS_DEFS_H

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#include <inttypes.h>
#include <string.h>

typedef unsigned char u8;
typedef signed char s8;

typedef unsigned short u16;
typedef signed short s16;

typedef unsigned int u32;
typedef signed int s32;

typedef uint64_t u64;
typedef int64_t s64;

static inline int streq(const char *a, const char *b)
{
	return strcmp(a, b) == 0;
}

#define min(a,b)				\
	({ __typeof__ (a) _a = (a);		\
		__typeof__ (b) _b = (b);	\
		_a < _b ? _a : _b; })

#define max(a,b)				\
	({ __typeof__ (a) _a = (a);		\
		__typeof__ (b) _b = (b);	\
		_a > _b ? _a : _b; })



#endif /* LNVIS_DEFS_H */

