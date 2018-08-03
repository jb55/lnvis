
#ifndef LNVIS_LN_H
#define LNVIS_LN_H

#include "nanovg/nanovg.h"

struct node {
	const char *alias;
	const char *nodeid;

	union {
		float rgba[4];
		struct {
			float r, g, b, a;
		};
	} color;
};

struct ln {
	NVGcontext *vg;
	struct node *nodes;
	int node_count;
};

#endif /* LNVIS_LN_H */
