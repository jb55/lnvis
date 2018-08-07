
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
			assert(cell->node_count < CELL_MAX_ELEMS);
			return;
		}
		else {
			assert(i + 1 <= CELL_MAX_ELEMS);
			int msize = sizeof(struct node *) * (cell->node_count - i - 1);
			memmove(n, n + 1, msize);
			cell->node_count--;
			assert(cell->node_count < CELL_MAX_ELEMS);
			return;
		}
	}

	/* assert(!"expect to remove a node"); */
}

void update_grid_move_nodes(struct ln *ln) {
	struct node *n = NULL;
	struct cell *new_cell;

	for (u32 i = 0; i < ln->node_count; i++) {
		n = &ln->nodes[i];

		int gx = ((double)(n->x / (double)ln->window_width)) * ln->grid_div;
		int gy = ((double)(n->y / (double)ln->window_height)) * ln->grid_div;

		// TODO: handle outside of grid?
		if (gx > ln->grid_div || gy > ln->grid_div)
			continue;

		new_cell = &ln->grid[gy * ln->grid_div + gx];

		// we've moved to a new cell
		if (n->cell != new_cell) {
			// can't move to new cell :[
			if (new_cell->node_count == CELL_MAX_ELEMS)
				continue;

			// remove from old cell
			if (n->cell)
				remove_node_from_cell(n->cell, n);

			// add to new cell
			n->cell = new_cell;

			/* printf("adding to cell (%d, %d) elems %d\n", */
			/*        gx, gy, new_cell->node_count+1); */
			new_cell->nodes[new_cell->node_count++] = n;
		}
	}
}




