
#include "render.h"
#include "defs.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "nanovg/nanovg.h"

void draw_channel(NVGcontext *vg, struct ln *ln, struct channel *channel)
{
	const struct node *n1 = channel->nodes[0];
	const struct node *n2 = channel->nodes[1];

	if (!(n1->filtered && n2->filtered))
		return;

	const float stroke = max(1.0, channel->satoshis * 0.000001f);
	/* const float stroke = (logf(channel->satoshis) / logf(10)) * 0.0f; */

	const float sx = n1->x;
	const float sy = n1->y;

	const float ex = n2->x;
	const float ey = n2->y;

	union color n1t, n2t;

	n1t.nvg_color = n1->color.nvg_color;
	n2t.nvg_color = n2->color.nvg_color;

	NVGcolor c;

	if (nodeid_eq(channel->source, n1->id))
		c = n1t.nvg_color;
	else
		c = n2t.nvg_color;

	if (channel->nodes[0] == ln->last_drag_target ||
	    channel->nodes[1] == ln->last_drag_target)
		c.a = 1.0;
	else
		c.a = 0.4;


	/* NVGpaint linear_grad = */
	/* 	nvgLinearGradient(vg, sx, sy, ex, ey, n1t.nvg_color, n2t.nvg_color); */

	nvgSave(vg);
	nvgStrokeWidth(vg, stroke);

	/* nvgStrokePaint(vg, linear_grad); */
	nvgStrokeColor(vg, c);
	nvgBeginPath(vg);
	nvgMoveTo(vg, n1->x, n1->y);
	nvgLineTo(vg, n2->x, n2->y);
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
	if (!node->filtered)
		return;

	nvgFontSize(vg, 18.0f);
	nvgFontFace(vg, "sans");
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

	const float r = node->size;
	float bounds[4];
	static const float pad = 2.0f;

	nvgTextBounds(vg, 0, 0, node->alias, NULL, &bounds);

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
	if (!dark_theme)
		bg = nvgRadialGradient(vg, -light, -light, 0, r+2.0,
				       ln->clear_color.nvg_color,
				       node_color);
	else
		bg = nvgRadialGradient(vg, -light, -light, 0, r+2.0, node_color, blend);

	nvgBeginPath(vg);
	nvgCircle(vg, 0, 0, r);
	/* nvgPathWinding(vg, NVG_HOLE); */

	if (ln->display_flags & DISP_STROKE_NODES) {
		nvgStrokeWidth(vg, 3.0f);
		nvgStrokeColor(vg, dark_theme ? clear_adj : ln->clear_color.nvg_color);
		nvgStroke(vg);
	}

	nvgFillPaint(vg, bg);
	nvgFill(vg);

	if (ln->display_flags & DISP_ALIASES) {

		nvgBeginPath(vg);
		nvgRoundedRect(vg, -bounds[2] / 2.0 - pad, bounds[3] - pad, bounds[2] + pad * 2.0, bounds[3] + pad * 2.0, 5.0);
		nvgFillColor(vg, nvgRGBAf(node_color.r, node_color.g, node_color.b, 0.9));
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

	struct channel *draw_last = NULL, *c = NULL;

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
