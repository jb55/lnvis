
#include "ln.h"
#include "grid.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

struct node *hit_node(struct ln *ln) {
	for (u32 i = 0; i < ln->node_count; ++i)
	{
		struct node *n = &ln->nodes[i];

		const double dx = fabs(n->x - ln->mx);
		const double dy = fabs(n->y - ln->my);

		const double d = sqrt(dx*dx + dy*dy);

		/* printf("%s's dx %f dy %f d %f\n", n->alias, dx, dy, d); */

		if (d <= n->size)
			return n;
	}

	return NULL;
}


void repel_nodes(struct node *n1, struct node *n2, double dt) {
	double dx = n2->x - n1->x;
	double dy = n2->y - n1->y;

	double d = sqrt(dx*dx + dy*dy);

	static const double mindist = 200.0;
	if (d < mindist) {

		// normalized vector between two nodes
		double nx = dx / d;
		double ny = dy / d;

		// get the distance past the minimum distance along
		// the vector between the two nodes
		double nnx = (d - mindist) * nx;
		double nny = (d - mindist) * ny;

		if (isnan(nnx) || isnan(nny))
			return;

		// normalize by frame time
		double mx = nnx * dt;
		double my = nny * dt;

		// correct for the distance by accelerating the node away
		// from the other node
		n1->vx += mx;
		n1->vy += my;

		// do the same in the opposite direction for the other node
		n2->vx -= mx;
		n2->vy -= my;
	}
	else {
		const double scale = 1.0;

		n1->vx += dx * scale * dt;
		n1->vy += dy * scale * dt;

		n2->vx += -dx * scale * dt;
		n2->vy += -dy * scale * dt;
	}
}


static void force_graph(struct ln *ln, double dt) {
	u32 i;

	// channel constraints
	for (i = 0; i < ln->channel_count; ++i) {
		struct channel *channel = &ln->channels[i];

		struct node *n1 = channel->nodes[0];
		struct node *n2 = channel->nodes[1];

		if (!n1->filtered || !n2->filtered)
			continue;

		repel_nodes(n1, n2, dt);
	}
}


static void repel_nearby(struct node *node, double dt)
{
	struct node *n = NULL;
	struct cell *cell = node->cell;

	// might happen the first iteration?
	if (cell == NULL)
		return;

	// we're the only one in this cell, there's nothing to repel
	if (cell->node_count == 1)
		return;

	for (int i = 0; i < cell->node_count; ++i) {
		n = cell->nodes[i];

		// dont repel against ourselves
		if (n == node)
			continue;

		assert(n);
		assert(node);
		repel_nodes(n, node, dt);
	}
}


static void physics(struct ln *ln, double dt)
{
	static const double friction_coeff = 0.03;
	static const double friction = 1.0 - friction_coeff;

	// physics
	for (u32 i = 0; i < ln->node_count; i++) {
		struct node *node = &ln->nodes[i];

		if (!node->filtered)
			continue;

		/* repel_nearby(node, dt); */

		// semi-implicit euler
		node->ax *= friction;
		node->ay *= friction;

		// update node position
		node->vx += node->ax * dt;
		node->vy += node->ay * dt;

		// forces (testing)
		node->vx *= friction;
		node->vy *= friction;

		node->x += node->vx * dt;
		node->y += node->vy * dt;
	}
}



// force graph update logic
void update(struct ln *ln, double dt)
{
	// click detection
	if (ln->clicked) {
		struct node *hit = hit_node(ln);
		ln->drag_target = hit;
		ln->last_drag_target = hit;
	}

	// stop dragging
	if (!ln->mdown && ln->drag_target) {
		ln->last_drag_target = ln->drag_target;
		ln->drag_target = NULL;
	}

	// drag
	if (ln->mdown && ln->drag_target) {
		ln->drag_target->x = ln->mx;
		ln->drag_target->y = ln->my;
		ln->drag_target->vx = 0;
		ln->drag_target->vy = 0;
	}

	/* force_graph(ln, dt); */

	/* physics(ln, dt); */

	/* update_grid_move_nodes(ln); */
}
