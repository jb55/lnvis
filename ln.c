
#include "ln.h"
#include "grid.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void init_ln(struct ln *ln, int grid_div) {
	ln->clicked = 0;
	ln->grid_div = grid_div;
	ln->grid = calloc(ln->grid_div * ln->grid_div, sizeof(*ln->grid));
}

void free_ln(struct ln *ln) {
	free(ln->grid);
}

static double rand_0to1() {
	return (double) rand() / RAND_MAX;
}

void random_network(int ww, int wh, int max_per_node, int num_nodes, struct ln *ln) {
	int i, j;
	int from, to;
	int tries = 0;
	struct node *n;
	u8 connections[num_nodes][num_nodes];
	memset(connections, 0, sizeof(connections));

	ln->nodes = calloc(sizeof(*ln->nodes), num_nodes);
	ln->channel_count = 0;
	ln->node_count = num_nodes;
	ln->channels = calloc(sizeof(*ln->channels), num_nodes * max_per_node);

	printf("max channels %d\n", num_nodes * max_per_node);

	for (i = 0; i < num_nodes; ++i) {
		n = &ln->nodes[i];

		n->alias = "test";
		n->color.r = rand_0to1();
		n->color.g = rand_0to1();
		n->color.b = rand_0to1();
		n->color.a = 1.0;
		n->x = ww * rand_0to1();
		n->y = wh * rand_0to1();
		n->ax = 0.0;
		n->ay = 0.0;
		n->vx = 0.0;
		n->vy = 0.0;
		n->size = 10;
	}

	// connect nodes randomly
	for (i = 0; i < num_nodes; ++i) {

		from = i;
	skip:
		// for each node, it can have 0 to max_per_node connections
		for (j = 0; j < rand() % max_per_node; ++j) {
			do {
				tries++;
				// if connections are way higher than the
				// possible number of nodes for some reason,
				// we'll need an escape hatch
				if (tries > 5) {
					tries = 0;
					goto skip;
				}
				to = rand() % num_nodes;
			}
			while(connections[from][to]);
			tries = 0;

			connections[from][to] = 1;

			struct channel *c =
				&ln->channels[ln->channel_count++];

			c->nodes[0] = &ln->nodes[from];
			c->nodes[1] = &ln->nodes[to];

		}

		// keep trying until we find on that isn't already connected
	}
}
