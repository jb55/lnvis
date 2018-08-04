
#include "ln.h"
#include <math.h>
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

// force graph update logic
void update(struct ln *ln, double dt)
{


	// click detection
	if (ln->clicked) {
		struct node *hit = hit_node(ln);
		ln->drag_target = hit;
	}

	// stop dragging
	if (!ln->mdown && ln->drag_target)
		ln->drag_target = NULL;

	// drag
	if (ln->mdown && ln->drag_target) {
		ln->drag_target->x = ln->mx;
		ln->drag_target->y = ln->my;
		ln->drag_target->vx = 0;
		ln->drag_target->vy = 0;
	}

	u32 i;
	static const double friction = 0.3;

	for (i = 0; i < ln->channel_count; ++i) {
		struct channel *channel = &ln->channels[i];

		struct node *n1 = channel->nodes[0];
		struct node *n2 = channel->nodes[1];

		double dx = n2->x - n1->x;
		double dy = n2->y - n1->y;

		n1->vx += dx * 0.001;
		n1->vy += dy * 0.001;
	}

	for (u32 i = 0; i < ln->node_count; i++) {
		struct node *node = &ln->nodes[i];

		// semi-implicit euler

		// update node position
		node->vx += node->ax * dt;
		node->vy += node->ay * dt;

		// forces (testing)
		if (node->vx > 0.0)
			node->vx -= friction;

		if (node->vy > 0.0)
			node->vy -= friction;

		node->x += node->vx * dt;
		node->y += node->vy * dt;

	}
}
