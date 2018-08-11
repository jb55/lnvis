
#include "jsmn.h"
#include "json.h"
#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_READING_CHANNELS -2
#define ERROR_ALLOCATING_CHANNELS -3

struct parser {
	const char *field;
	enum parsing_state state;
};

/* "source": "02fc7ef232bd958209a2180fb18844388babc48a81d31434e343ecc4d7dd1a9186",  */
/* "destination": "03b7ca940bc33b882dc1f1bee353a6cf205b1a7472d8ae24d45370a8f510c27d23",  */
/* "short_channel_id": "505273:1478:0",  */
/* "public": true,  */
/* "satoshis": 80000,  */
/* "flags": 0,  */
/* "active": true,  */
/* "last_update": 1531178014,  */
/* "base_fee_millisatoshi": 1,  */
/* "fee_per_millionth": 1,  */
/* "delay": 14 */
static const struct parser channel_parsers[] = {
	{ "short_channel_id",      PARSING_SHORTID },
	{ "public",                PARSING_PUBLIC },
	{ "satoshis",              PARSING_SATOSHIS },
	{ "flags",                 PARSING_FLAGS },
	{ "active",                PARSING_ACTIVE },
	{ "last_update",           PARSING_LAST_UPDATE },
	{ "base_fee_millisatoshi", PARSING_BASE_FEE },
	{ "fee_per_millionth",     PARSING_FEE_PER },
	{ "delay",                 PARSING_DELAY },
};

#define MKTMPBUF \
	char buf[toklen + 1]; \
	buf[toklen] = '\0'; \
	memcpy(buf, tokstr, toklen)

static inline u32 parse_u32(int toklen, const char *tokstr)
{
	MKTMPBUF;
	return strtoul(buf, NULL, 10);
}

static inline u64 parse_u64(int toklen, const char *tokstr)
{
	MKTMPBUF;
	return strtoll(buf, NULL, 10);
}

static int parse_short_channel_id(int toklen, const char *tokstr,
				   struct short_channel_id *chanid)
{
	char buf[toklen + 1];
	buf[toklen] = '\0';
	memcpy(buf, tokstr, toklen);
	return sscanf(buf, "%u:%u:%hu",
		      &chanid->blocknum,
		      &chanid->txnum,
		      &chanid->outnum);
}

static inline int tokeq(size_t toklen, const char* tok, const char *str)
{
	if (toklen != strlen(str))
		return 0;
	return memcmp(tok, str, toklen) == 0;
}

int parse_json(FILE *fd, jsmntok_t **ptoks, int *ntoks, char **pbuffer) {
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

	*ntoks = parserr;
	*ptoks = toks;
	*pbuffer = buffer;

	return 0;
}

int parse_clightning_channels(FILE *fd, int *nchannels, struct channel **pchannels)
{
	char *buffer;
	void *res;
	struct channel *chan = NULL, *channels;
	int nchans = 0;
	int chancap = 4096;
	jsmntok_t *toks;
	jsmntok_t *tok;
	char *tokstr;
	int ntoks;
	int toklen;
	enum parsing_state state = PARSING_TOKEN;
	int rez = parse_json(fd, &toks, &ntoks, &buffer);
	if (rez < 0)
		return rez;

	channels = calloc(chancap, sizeof(struct channel));

	int objs = 0;

	// parse tokens
	for (int i = 0; i < ntoks; ++i) {
		tok = &toks[i];

		if (tok->type == JSMN_ARRAY)
			continue;

		if (tok->type == JSMN_OBJECT) {
			if (++objs > 2)
				nchans++;

			continue;
		}

		toklen = tok->end - tok->start;
		tokstr = &buffer[tok->start];

		// allocate more channels if needed
		if (nchans > chancap) {
			chancap *= 2;
			res = realloc(channels, chancap * sizeof(struct channel));
			if (res == NULL) {
				free(channels);
				return ERROR_ALLOCATING_CHANNELS;
			}
			channels = res;
		}

		chan = &channels[nchans];

		// TODO: lookup node id, assign nodes
		switch (state) {
		case PARSING_SHORTID:
			parse_short_channel_id(toklen, tokstr,
					       &chan->short_channel_id);
			state = PARSING_TOKEN;
			break;

		case PARSING_PUBLIC:
			chan->public = tokeq(toklen, tokstr, "true");
			state = PARSING_TOKEN;
			break;

		case PARSING_SATOSHIS:
			chan->satoshis = parse_u64(toklen, tokstr);
			state = PARSING_TOKEN;
			break;

		case PARSING_FLAGS:
			chan->flags = parse_u32(toklen, tokstr);
			state = PARSING_TOKEN;
			break;

		case PARSING_ACTIVE:
			chan->active = tokeq(toklen, tokstr, "true");
			state = PARSING_TOKEN;
			break;

		case PARSING_LAST_UPDATE:
			chan->last_update_timestamp = parse_u32(toklen, tokstr);
			state = PARSING_TOKEN;
			break;

		case PARSING_BASE_FEE:
			chan->base_fee_msat = parse_u32(toklen, tokstr);
			state = PARSING_TOKEN;
			break;

		case PARSING_FEE_PER:
			chan->fee_per_millionth = parse_u32(toklen, tokstr);
			state = PARSING_TOKEN;
			break;

		case PARSING_DELAY:
			chan->delay = parse_u32(toklen, tokstr);
			state = PARSING_TOKEN;
			break;

		case PARSING_TOKEN:
			for (size_t i = 0; i < ARRAY_SIZE(channel_parsers); i++) {
				if (tokeq(toklen, tokstr,
					  channel_parsers[i].field)) {
					state = channel_parsers[i].state;
					continue;
				}
			}
			break;
		}

	}

	*pchannels = channels;
	*nchannels = nchans + 1;

	free(buffer);
	free(toks);

	return 0;
}
