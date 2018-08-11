

#ifndef LNVIS_JSON_H
#define LNVIS_JSON_H

#include <stdio.h>
#include "ln.h"

enum nodes_parsing_state {
	PARSING_NODE_TOKEN
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
};

int parse_clightning_channels(FILE *fd, int *nchannels,
			      struct channel **channels);

#endif /* LNVIS_JSON_H */
