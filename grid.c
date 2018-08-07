
#include "grid.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>


static void remove_node_from_cell(struct cell *cell, struct node *node) {
	struct node **n = NULL;

	for (int i = 0; i < cell->node_count; i++) {
		n = &cell->nodes[i];
		if (node != *n)
			continue;

		// shortcut: reduce node count if it's the last node
		if (i == cell->node_count-1) {
			cell->node_count--;
			return;
		}
		else {
			assert(i + 1 < CELL_MAX_ELEMS);
			memmove(n, n + 1,
				sizeof(*cell->nodes) * (cell->node_count - 1));
			cell->node_count--;
			return;
		}
	}

	/* assert(!"expect to remove a node"); */
}

void update_grid_move_nodes(struct ln *ln) {
	struct node *n = NULL;
	struct cell *new_cell, *old_cell;

	for (u32 i = 0; i < ln->node_count; i++) {
		n = &ln->nodes[i];

		int gx = ((double)(n->x / (double)ln->window_width)) * ln->grid_div;
		int gy = ((double)(n->y / (double)ln->window_height)) * ln->grid_div;

		// TODO: handle outside of grid?
		if (gx > ln->grid_div || gy > ln->grid_div)
			continue;

		// we've moved to a new cell
		if (gx != n->grid_x || gy != n->grid_y) {
			new_cell = &ln->grid[gy * ln->grid_div + gx];

			// can't move to new cell :[
			if (new_cell->node_count == CELL_MAX_ELEMS)
				continue;

			old_cell = &ln->grid[n->grid_y * ln->grid_div + n->grid_x];

			// remove from old cell
			remove_node_from_cell(old_cell, n);

			// add to new cell
			n->grid_x = gx;
			n->grid_y = gy;

			new_cell->nodes[new_cell->node_count++] = n;
		}
	}
}



