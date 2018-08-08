
#include "render.h"
#include <stdio.h>
#include <assert.h>
#include "nanovg/nanovg.h"

void draw_channel(NVGcontext *vg, struct channel *channel)
{
	const struct node *n1 = channel->nodes[0];
	const struct node *n2 = channel->nodes[1];
	static const float stroke = 2.0f;

	const float sx = n1->x;
	const float sy = n1->y;

	const float ex = n2->x;
	const float ey = n2->y;

	NVGpaint linear_grad =
		nvgLinearGradient(vg, sx, sy, ex, ey,
				  n1->color.nvg_color,
				  n2->color.nvg_color);

	nvgSave(vg);
	nvgStrokeWidth(vg, stroke);

	nvgStrokePaint(vg, linear_grad);
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
	const float r = node->size;
	/* const float pos = 500.0; */

	/* const float h = r; */
	/* const float w = r; */
	/* const float x = pos; */
	/* const float y = pos; */
	/* const float kr = (int)(h * 0.35f); */
	/* const float cy = y + (int)(h * 0.5f); */
	NVGpaint bg;

	NVGcolor node_color =
		nvgRGBAf(node->color.r, node->color.g, node->color.b,
			 node->color.a);

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

	// TODO: use brightness instead of clear color for white theme
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
		nvgFontSize(vg, 18.0f);
		nvgFontFace(vg, "sans");
		nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
		/* nvgTextMetrics(vg, NULL, NULL, &lineh); */
		nvgFillColor(vg, node_color);
		nvgText(vg, -r, r, node->alias, NULL);
	}

	nvgRestore(vg);
}

void render_ln(struct ln *ln)
{
	u32 i;
	NVGcontext *vg = ln->vg;

	draw_grid(vg, ln);

	// render channels first
	for (i = 0; i < ln->channel_count; i++)
		draw_channel(vg, &ln->channels[i]);

	for (i = 0; i < ln->node_count; i++)
		draw_node(vg, ln, &ln->nodes[i]);
}
