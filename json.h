

#ifndef LNVIS_JSON_H
#define LNVIS_JSON_H

#include <stdio.h>
#include "ln.h"

enum parsing_state {
	PARSING_TOKEN,
	PARSING_SHORTID,
	PARSING_PUBLIC,
	PARSING_SATOSHIS,
	PARSING_FLAGS,
	PARSING_ACTIVE,
	PARSING_LAST_UPDATE,
	PARSING_BASE_FEE,
	PARSING_FEE_PER,
	PARSING_DELAY,
};

int parse_clightning_channels(FILE *fd, int *nchannels,
			      struct channel **channels);

#endif /* LNVIS_JSON_H */
