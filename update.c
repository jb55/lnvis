
#include "ln.h"
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

// force graph update logic
void update(struct ln *ln, double dt)
{
	u32 i;
	static const double friction_coeff = 0.03;
	static const double friction = 1.0 - friction_coeff;

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

	// channel constraints
	for (i = 0; i < ln->channel_count; ++i) {
		struct channel *channel = &ln->channels[i];

		struct node *n1 = channel->nodes[0];
		struct node *n2 = channel->nodes[1];

		double dx = n2->x - n1->x;
		double dy = n2->y - n1->y;

		double d = sqrt(dx*dx + dy*dy);

		static const double mindist = 100.0;
		if (d < mindist) {
			double nx = dx / d;
			double ny = dy / d;

			double nnx = (d - mindist) * nx;
			double nny = (d - mindist) * ny;

			double mx = nnx;
			double my = nny;

			n1->vx += mx;
			n1->vy += my;

			n2->vx -= mx;
			n2->vy -= my;
		}
		else {
			const double scale = 0.01;

			n1->vx += dx * scale;
			n1->vy += dy * scale;

			n2->vx += -dx * scale;
			n2->vy += -dy * scale;
		}
	}

	for (u32 i = 0; i < ln->node_count; i++) {
		struct node *node = &ln->nodes[i];

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
