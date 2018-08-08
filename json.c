
#include "jsmn.h"
#include <stdio.h>
#include <assert.h>

#define NUM_TOKENS 2048
#define READ_BUFSIZE 4096

int parse_clightning_channels(FILE *fd)
{
	static char buffer[READ_BUFSIZE] = {0};
	static jsmntok_t tokens[NUM_TOKENS];
	jsmntok_t *token;
	int ntok = 0;
	size_t bytes = 0;
	int count = 1;

	jsmn_parser parser;
	jsmn_init(&parser);

	while ((bytes = fread(buffer, 1, READ_BUFSIZE, fd))) {
		int parse_err = jsmn_parse(&parser, buffer, bytes, tokens, NUM_TOKENS, &ntok);

		printf("parse_err %d ntok %d\n", parse_err, ntok);

		if (parse_err != JSMN_ERROR_PART && parse_err < 0)
			return parse_err;

		for (int i = 0; i < ntok; ++i, count++) {
			token = &tokens[i];
			if (token->type == JSMN_ARRAY || token->type == JSMN_OBJECT)
				continue;
			printf("%d %d: %.*s\n",
			       token->type,
			       count,
			       token->end - token->start,
			       &buffer[token->start]);
		}
	}

	return ntok;
}
