
#include "render.h"
#include "defs.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "nanovg/nanovg.h"
#include <stdlib.h>


#define  Pr  .299
#define  Pg  .587
#define  Pb  .114

static inline double rand_0to1() {
	return (double) rand() / RAND_MAX;
}


void saturate(union color *c, double change)
{
	double  P=sqrt(
		(c->r)*(c->r)*Pr+
		(c->g)*(c->g)*Pg+
		(c->b)*(c->b)*Pb ) ;

	c->r = P+((c->r)-P)*change;
	c->g = P+((c->g)-P)*change;
	c->b = P+((c->b)-P)*change;
}

void draw_channel(NVGcontext *vg, struct ln *ln, struct channel *channel)
{
	if (!channel->visible)
		return;

	const struct node *n1 = channel->nodes[0];
	const struct node *n2 = channel->nodes[1];

	if (!n1->visible || !n2->visible)
		return;

	const float stroke = max(1.0, channel->satoshis * 0.000001f);
	/* const float stroke = (logf(channel->satoshis) / logf(10)) * 0.0f; */

	union color n1t, n2t;

	n1t.nvg_color = n1->color.nvg_color;
	n2t.nvg_color = n2->color.nvg_color;

	NVGcolor c;

	if (nodeid_eq(channel->source, n1->id))
		c = n1t.nvg_color;
	else
		c = n2t.nvg_color;

	if (channel->active && (channel->nodes[0] == ln->last_drag_target || channel->nodes[1] == ln->last_drag_target))
		c.a = 1.0;
	else {
		if (!channel->active || ln->drag_target) {
			saturate((union color*)&c, 0.01);
			c.a = 0.1;
		}
		else
			c.a = 0.4;
	}


	/* NVGpaint linear_grad = */
	/* 	nvgLinearGradient(vg, sx, sy, ex, ey, n1t.nvg_color, n2t.nvg_color); */

	nvgSave(vg);
	nvgStrokeWidth(vg, stroke);

	/* nvgStrokePaint(vg, linear_grad); */
	nvgStrokeColor(vg, c);
	nvgBeginPath(vg);

#define TAU (2.0*NVG_PI)



	nvgMoveTo(vg, n1->x, n1->y);

	if (ln->display_flags & DISP_BEZIER) {
		srand(channel->short_channel_id.blocknum ^
		      channel->short_channel_id.outnum ^
		      channel->short_channel_id.txnum);
		float ang1 = TAU*rand_0to1();
		float ang2 = TAU*rand_0to1();

		float dx = n2->x - n1->x;
		float dy = n2->y - n1->y;

		float len = sqrt(dx*dx + dy*dy);

		/* float nx = dx/len; */
		/* float ny = dy/len; */

		const float dist = 1.0/3.0;

		float ax1 = cos(ang1) * len * dist;
		float ay1 = sin(ang1) * len * dist;

		float ax2 = cos(ang2) * len * dist;
		float ay2 = sin(ang2) * len * dist;

		nvgBezierTo(vg, n1->x + ax1,
			        n1->y + ay1,
			        n2->x - ax2,
			        n2->y - ay2,
			        n2->x, n2->y);
	}
	else {
		nvgLineTo(vg, n2->x, n2->y);
	}
	/* nvgMoveTo(vg, n2->x, n2->y); */
	/* nvgArc(vg, n1->x, n1->y, TAU*4.0, TAU/8.0, 100.0, NVG_CCW); */
	/* nvgClosePath(vg); */
	/* nvgArcTo(vg, n1->x, n1->y, n2->x, n2->y, 100.0); */
	/* nvgLineTo(vg, n2->x, n2->y); */
	nvgStroke(vg);
	nvgRestore(vg);
}


void draw_grid(NVGcontext *vg, struct ln *ln) {
	static const float adj = 0.6f;
	const int grid_div = ln->grid_div;
	const int ww = ln->window_width;
	const int wh = ln->window_height;

	NVGcolor clear_adj =
		nvgRGBAf(ln->clear_color.r * adj,
			 ln->clear_color.g * adj,
			 ln->clear_color.b * adj,
			 1.0f);

	for (int x = 0; x < grid_div; ++x)  {
		double px = ww / grid_div * x;
			//int py = wh / grid_div * y;

		nvgBeginPath(vg);
		nvgMoveTo(vg, px, 0);
		nvgLineTo(vg, px, wh);
		nvgStrokeColor(vg, clear_adj);
		nvgStroke(vg);
	}

	for (int y = 0; y < grid_div; ++y)  {
		double py = wh / grid_div * y;
		//int py = wh / grid_div * y;

		nvgBeginPath(vg);
		nvgMoveTo(vg, 0, py);
		nvgLineTo(vg, ww, py);
		nvgStroke(vg);
	}
}


void draw_node(NVGcontext *vg, struct ln *ln, struct node *node)
{
	if (!node->visible)
		return;

	nvgFontSize(vg, 18.0f);
	nvgFontFace(vg, "sans");
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

	const float r = node->size;
	float bounds[4];
	static const float pad = 2.0f;

	nvgTextBounds(vg, 0, 0, node->alias, NULL, &bounds[0]);

	NVGpaint bg;

	/* if (streq(node->alias, "@jb55")) */
	/* 	printf("%f %f %f\n", node->color.r, */
	/* 		node->color.g, node->color.b); */

	NVGcolor node_color =
		nvgRGBf(node->color.r, node->color.g, node->color.b);

	NVGcolor node_color_inv =
		nvgRGBf(1.0-node->color.r, 1.0-node->color.g, 1.0-node->color.b);

	static const float adj = 0.3f;

	NVGcolor clear_adj =
		nvgRGBAf(ln->clear_color.r * adj,
			 ln->clear_color.g * adj,
			 ln->clear_color.b * adj,
			 1.0f);

	NVGcolor blend =
		nvgRGBAf(node->color.r * adj,
			 node->color.g * adj,
			 node->color.b * adj,
			 1.0f);

	nvgSave(vg);
	nvgTranslate(vg, node->x, node->y);

	const float light = 2.0f;
	const int dark_theme = ln->display_flags & DISP_DARK;

	//TODO: use brightness instead of clear color for white theme
	nvgBeginPath(vg);
	nvgCircle(vg, 0, 0, r);
	/* nvgPathWinding(vg, NVG_HOLE); */


	if (node->adj_drag_target || node == ln->last_drag_target) {
		node_color.a = 1.0;
		node_color_inv.a = 1.0;
		blend.a = 1.0;
		clear_adj.a = 1.0;
	}
	else {
		if (ln->drag_target) {
			saturate((union color*)&node_color, 0.01);
			node_color.a = 0.1;
			node_color_inv.a = 0.1;
			blend.a = 0.1;
			clear_adj.a = 0.1;
		}
		else {
			node_color.a = 0.9;
			node_color_inv.a = 0.9;
			blend.a = 0.9;
			clear_adj.a = 0.9;
		}
	}

	if (!dark_theme)
		bg = nvgRadialGradient(vg, -light, -light, 0, r+2.0,
				       ln->clear_color.nvg_color,
				       node_color);
	else
		bg = nvgRadialGradient(vg, -light, -light, 0, r+2.0, node_color, blend);

	if (ln->display_flags & DISP_STROKE_NODES) {
		nvgStrokeWidth(vg, 3.0f);
		nvgStrokeColor(vg, dark_theme ? clear_adj : ln->clear_color.nvg_color);
		nvgStroke(vg);
	}

	nvgFillPaint(vg, bg);
	nvgFill(vg);

	int is_adj = !ln->drag_target || ((ln->drag_target && node->adj_drag_target) || node == ln->drag_target);

	if (is_adj && ln->display_flags & DISP_ALIASES) {

		nvgBeginPath(vg);
		nvgRoundedRect(vg, -bounds[2] / 2.0 - pad, bounds[3] - pad, bounds[2] + pad * 2.0, bounds[3] + pad * 2.0, 5.0);
		nvgFillColor(vg, nvgRGBAf(node_color.r, node_color.g, node_color.b, node_color.a));
		nvgFill(vg);

		/* nvgTextMetrics(vg, NULL, NULL, &lineh); */
		nvgFillColor(vg, node_color_inv);
		nvgText(vg, -bounds[2] / 2.0, bounds[3], node->alias, NULL);
	}

	nvgRestore(vg);
}

void render_ln(struct ln *ln)
{
	u32 i;
	NVGcontext *vg = ln->vg;

	struct channel *c = NULL;

	if (ln->display_flags & DISP_GRID)
		draw_grid(vg, ln);

	// render channels first
	for (i = 0; i < ln->channel_count; i++) {
		c = &ln->channels[i];

		if (c->nodes[0] == ln->last_drag_target ||
		    c->nodes[1] == ln->last_drag_target)
			c->draw_last = 1;
		else
			draw_channel(vg, ln, c);
	}

	// draw important channels last
	for (i = 0; i < ln->channel_count; i++) {
		c = &ln->channels[i];

		if (!c->draw_last)
			continue;

		c->draw_last = 0;
		draw_channel(vg, ln, c);
	}


	for (i = 0; i < ln->node_count; i++)
		draw_node(vg, ln, &ln->nodes[i]);
}
