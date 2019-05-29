
#include "jsmn.h"
#include "json.h"
#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_READING_CHANNELS -2
#define ERROR_ALLOCATING_CHANNELS -3

struct channel_parser {
	const char *field;
	enum channel_parsing_state state;
};


struct node_parser {
	const char *field;
	enum node_parsing_state state;
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
static const struct channel_parser channel_parsers[] = {
	{ "source",                PARSING_CHAN_SOURCE },
	{ "destination",           PARSING_CHAN_DESTINATION },
	{ "short_channel_id",      PARSING_CHAN_SHORTID },
	{ "public",                PARSING_CHAN_PUBLIC },
	{ "satoshis",              PARSING_CHAN_SATOSHIS },
	{ "channel_flags",         PARSING_CHAN_FLAGS },
	{ "active",                PARSING_CHAN_ACTIVE },
	{ "last_update",           PARSING_CHAN_LAST_UPDATE },
	{ "base_fee_millisatoshi", PARSING_CHAN_BASE_FEE },
	{ "fee_per_millionth",     PARSING_CHAN_FEE_PER },
	{ "delay",                 PARSING_CHAN_DELAY },
};

/* "nodeid": "03a8334aba5660e241468e2f0deb2526bfd50d0e3fe808d882913e39094dc1a028", */
/* "alias": "cosmicApotheosis", */
/* "color": "33cccc", */
/* "last_timestamp": 1533693585, */
/* "global_features": "", */
/* "addresses": [ */
/* 	{ "type": "ipv4", */
/* 	  "address": "138.229.205.237", */
/* 	  "port": 9735 */
/* 	}] */
static const struct node_parser node_parsers[] = {
	{ "nodeid",    PARSING_NODE_ID },
	{ "alias",     PARSING_NODE_ALIAS },
	{ "color",     PARSING_NODE_COLOR },
	{ "addresses", PARSING_NODE_ADDRESSES },
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
	return sscanf(buf, "%ux%ux%hu",
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


static inline u8 hexdigit(char hex) {
	return (hex <= '9') ? hex - '0' : toupper(hex) - 'A' + 10 ;
}


static void parse_color(int toklen, const char *tokstr, union color *color) {
	color->a = 1.0f;

	if (toklen != 6)
		goto colorerr;

	for (int i = 0; i < 3; i++) {
		u8 c1 = tokstr[i*2];
		u8 c2 = tokstr[i*2+1];

		if (!isxdigit(c1) || !isxdigit(c2))
			goto colorerr;

		u8 val = hexdigit(c1) << 4 | hexdigit(c2);
		color->rgba[i] = ((float)val) / 255.0f;
	}

	return;
colorerr:
	color->r = 0.0f;
	color->g = 0.0f;
	color->b = 0.0f;
	return;
}

int parse_clightning_nodes(FILE *fd, int *node_count, struct node **pnodes)
{
	struct node *node = NULL, *nodes;
	char *buffer, *tokstr;
	void *res;
	jsmntok_t *toks;
	jsmntok_t *tok;
	int i, ntoks = 0, toklen = 0, objs = 0;
	int nodecap = 4096;
	enum node_parsing_state state = PARSING_NODE_TOKEN;

	*node_count = 0;

	nodes = calloc(nodecap, sizeof(struct node));

	int acount = 0;

	int rez = parse_json(fd, &toks, &ntoks, &buffer);
	if (rez < 0)
		return rez;

	for (i = 0; i < ntoks; i++) {
		tok = &toks[i];

		if (tok->type == JSMN_ARRAY) {
			if (state == PARSING_NODE_ADDRESSES) {
				if (++acount == 1) {
					acount = 0;
					state = PARSING_NODE_TOKEN;
				}
			}
			continue;
		}

		if (tok->type == JSMN_OBJECT) {
			if (state == PARSING_NODE_ADDRESSES)
				continue;

			if (++objs > 2)
				(*node_count)++;

			continue;
		}

		toklen = tok->end - tok->start;
		tokstr = &buffer[tok->start];

		node = &nodes[*node_count];

		// allocate more channels if needed
		if (*node_count > nodecap) {
			nodecap *= 2;
			res = realloc(nodes, nodecap * sizeof(struct channel));
			if (res == NULL) {
				free(nodes);
				return ERROR_ALLOCATING_CHANNELS;
			}
			nodes = res;
		}

		switch (state) {
		case PARSING_NODE_ID:
			strncpy(node->id, tokstr, min(toklen, PUBKEY_SIZE));
			state = PARSING_NODE_TOKEN;
			break;

		case PARSING_NODE_ALIAS:
			strncpy(node->alias, tokstr, min(toklen, MAX_ALIAS_SIZE));
			state = PARSING_NODE_TOKEN;
			break;

		case PARSING_NODE_COLOR:
			parse_color(toklen, tokstr, &node->color);
			state = PARSING_NODE_TOKEN;
			break;

		case PARSING_NODE_ADDRESSES:
			// TODO: parse addresses
			state = PARSING_NODE_TOKEN;
			break;

		case PARSING_NODE_TOKEN:
			for (size_t i = 0; i < ARRAY_SIZE(node_parsers); i++) {
				if (tokeq(toklen, tokstr,
					  node_parsers[i].field)) {
					state = node_parsers[i].state;
					continue;
				}
			}
			break;
		}

	}

	*pnodes = nodes;
	*node_count = *node_count + 1;

	return 0;
}


int parse_clightning_channels(FILE *fd, int *nchans, struct channel **pchannels)
{
	char *buffer;
	void *res;
	struct channel *chan = NULL, *channels;
	struct short_channel_id last_chan;
	int chancap = 4096;
	jsmntok_t *toks;
	jsmntok_t *tok;
	char *tokstr;
	int ntoks = 0, toklen = 0, objs = 0;
	enum channel_parsing_state state = PARSING_CHAN_TOKEN;

	int rez = parse_json(fd, &toks, &ntoks, &buffer);
	if (rez < 0)
		return rez;

	channels = calloc(chancap, sizeof(struct channel));

	// parse tokens
	for (int i = 0; i < ntoks; ++i) {
		tok = &toks[i];

		if (tok->type == JSMN_ARRAY)
			continue;

		if (tok->type == JSMN_OBJECT) {
			if (++objs > 2)
				(*nchans)++;

			continue;
		}

		toklen = tok->end - tok->start;
		tokstr = &buffer[tok->start];

		// allocate more channels if needed
		if (*nchans >= chancap) {
			chancap *= 2;
			res = realloc(channels, chancap * sizeof(struct channel));
			if (res == NULL) {
				free(channels);
				return ERROR_ALLOCATING_CHANNELS;
			}
			channels = res;
		}

		chan = &channels[*nchans];

		switch (state) {
		case PARSING_CHAN_SOURCE:
			strncpy(chan->source, tokstr, min(PUBKEY_SIZE, toklen));
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_DESTINATION:
			strncpy(chan->destination, tokstr, min(PUBKEY_SIZE, toklen));
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_SHORTID:
			parse_short_channel_id(toklen, tokstr,
					       &chan->short_channel_id);
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_PUBLIC:
			chan->public = tokeq(toklen, tokstr, "true");
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_SATOSHIS:
			chan->satoshis = parse_u64(toklen, tokstr);
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_FLAGS:
			chan->flags = parse_u32(toklen, tokstr);
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_ACTIVE:
			chan->active = tokeq(toklen, tokstr, "true");
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_LAST_UPDATE:
			chan->last_update_timestamp = parse_u32(toklen, tokstr);
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_BASE_FEE:
			chan->base_fee_msat = parse_u32(toklen, tokstr);
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_FEE_PER:
			chan->fee_per_millionth = parse_u32(toklen, tokstr);
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_DELAY:
			chan->delay = parse_u32(toklen, tokstr);
			state = PARSING_CHAN_TOKEN;
			break;

		case PARSING_CHAN_TOKEN:
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
	*nchans = *nchans + 1;

	free(buffer);
	free(toks);

	return 0;
}
