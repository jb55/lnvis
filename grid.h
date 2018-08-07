
#ifndef LNVIS_GRID_H
#define LNVIS_GRID_H


#include "ln.h"

struct cell {
	struct node *nodes[CELL_MAX_ELEMS];
	int node_count;
};

void update_grid_move_nodes(struct ln *ln);


#endif /* LNVIS_GRID_H */
