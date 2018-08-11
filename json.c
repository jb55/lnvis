
#include "jsmn.h"
#include "ln.h"
#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR_READING_CHANNELS -2
#define ERROR_ALLOCATING_CHANNELS -3

int parse_clightning_channels(FILE *fd, int *nchannels, struct channel **channels)
{
	int off, parserr, i;
	void *res;
	char *buffer;
	jsmntok_t *toks;
	jsmn_parser parser;
	jsmn_init(&parser);

	int bufsize = 4096 * 4096;
	int toksize = 512 * 4096;

	buffer = malloc(bufsize);
	toks = calloc(toksize, sizeof(jsmntok_t));
	off = 0;
	parserr = 0;

	while (parserr <= 0) {
		/* Read more if parser says, or we have 0 tokens. */
		if (parserr == 0 || parserr == JSMN_ERROR_PART) {
			i = fread(buffer + off, 1, bufsize - 1 - off, fd);
			if (i <= 0)
				return ERROR_READING_CHANNELS;
			off += i;
			/* NUL terminate */
			buffer[off] = '\0';
		}

		/* (Continue) parsing */
		parserr = jsmn_parse(&parser, buffer, off, toks, toksize);

		switch (parserr) {
		case JSMN_ERROR_INVAL:
			return ERROR_READING_CHANNELS;
		case JSMN_ERROR_NOMEM:
			/* Need more tokens, double it */
			toksize *= 2;
			res = realloc(toks, toksize * sizeof(jsmntok_t));
			if (res == NULL) {
				free(toks);
				return ERROR_ALLOCATING_CHANNELS;
			}
			toks = res;
			break;
		case JSMN_ERROR_PART:
			/* Need more data: make room if necessary */
			if (off == bufsize - 1) {
				bufsize *= 2;
				res = realloc(buffer, bufsize);
				if (res == NULL) {
					free(buffer);
					return ERROR_ALLOCATING_CHANNELS;
				}
				buffer = res;
			}
			break;
		}
	}

	printf("got %d tokens\n", parserr);

	// parse tokens
	for (i = 0; i < parserr; ++i) {
	}

	free(buffer);
	free(toks);

	return 0;
}
