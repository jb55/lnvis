

#ifndef LNVIS_JSON_H
#define LNVIS_JSON_H

#include <stdio.h>
#include "ln.h"

enum node_parsing_state {
	PARSING_NODE_TOKEN,
	PARSING_NODE_ALIAS,
	PARSING_NODE_COLOR,
	PARSING_NODE_ID,
	PARSING_NODE_ADDRESSES,
};

enum channel_parsing_state {
	PARSING_CHAN_TOKEN,
	PARSING_CHAN_SHORTID,
	PARSING_CHAN_PUBLIC,
	PARSING_CHAN_SATOSHIS,
	PARSING_CHAN_FLAGS,
	PARSING_CHAN_ACTIVE,
	PARSING_CHAN_LAST_UPDATE,
	PARSING_CHAN_BASE_FEE,
	PARSING_CHAN_FEE_PER,
	PARSING_CHAN_DELAY,
	PARSING_CHAN_SOURCE,
	PARSING_CHAN_DESTINATION,
};

int parse_clightning_nodes(FILE *fd, int *node_count, struct node **nodes);
int parse_clightning_channels(FILE *fd, int *nchannels, struct channel **channels);

#endif /* LNVIS_JSON_H */
