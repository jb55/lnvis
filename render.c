
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

	NVGcolor n1_color =
		nvgRGBAf(n1->color.r,
			 n1->color.g,
			 n1->color.b,
			 n1->color.a);

	NVGcolor n2_color =
		nvgRGBAf(n2->color.r,
			 n2->color.g,
			 n2->color.b,
			 n2->color.a);

	NVGpaint linear_grad =
		nvgLinearGradient(vg, sx, sy, ex, ey, n1_color, n2_color);

	nvgSave(vg);
	nvgStrokeWidth(vg, stroke);

	nvgStrokePaint(vg, linear_grad);
	nvgBeginPath(vg);
	nvgMoveTo(vg, n1->x, n1->y);
	nvgLineTo(vg, n2->x, n2->y);
	nvgStroke(vg);
	nvgRestore(vg);
}


void draw_grid(NVGcontext *vg, int ww, int wh, int grid_div) {

	for (int x = 0; x < grid_div; ++x)  {
		double px = ww / grid_div * x;
			//int py = wh / grid_div * y;

		nvgBeginPath(vg);
		nvgMoveTo(vg, px, 0);
		nvgLineTo(vg, px, wh);
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

	NVGcolor clear =
		nvgRGBAf(ln->clear_color.r,
			 ln->clear_color.g,
			 ln->clear_color.b,
			 1.0f);

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

	// TODO: use brightness instead of clear color for white theme
	if (!ln->dark_theme)
		bg = nvgRadialGradient(vg, -light, -light, 0, r+2.0, clear, node_color);
	else
		bg = nvgRadialGradient(vg, -light, -light, 0, r+2.0, node_color, blend);

	nvgBeginPath(vg);
	nvgCircle(vg, 0, 0, r);
	/* nvgPathWinding(vg, NVG_HOLE); */

	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, ln->dark_theme ? clear_adj : clear);
	nvgStroke(vg);

	nvgFillPaint(vg, bg);
	nvgFill(vg);


	nvgFontSize(vg, 18.0f);
	nvgFontFace(vg, "sans");
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	/* nvgTextMetrics(vg, NULL, NULL, &lineh); */
	nvgFillColor(vg, node_color);
	nvgText(vg, -r, r, node->alias, NULL);

	nvgRestore(vg);
}

void render_ln(struct ln *ln)
{
	u32 i;
	NVGcontext *vg = ln->vg;

	draw_grid(vg, ln->window_width, ln->window_height, ln->grid_div);

	// render channels first
	for (i = 0; i < ln->channel_count; i++)
		draw_channel(vg, &ln->channels[i]);

	for (i = 0; i < ln->node_count; i++)
		draw_node(vg, ln, &ln->nodes[i]);
}
