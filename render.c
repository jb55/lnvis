
#include "render.h"
#include <stdio.h>
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

void draw_node(NVGcontext *vg, struct node *node)
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

	NVGcolor blend =
		nvgRGBAf(0, 0, 0, 1.0);

	nvgSave(vg);
	nvgTranslate(vg, node->x, node->y);

	const float light = 2.0f;
	bg = nvgRadialGradient(vg, -light, -light, 0, r+2.0, node_color, blend);
	nvgBeginPath(vg);
	nvgCircle(vg, 0, 0, r);
	/* nvgPathWinding(vg, NVG_HOLE); */

	nvgStrokeWidth(vg, 3.0f);
	nvgStrokeColor(vg, nvgRGBf(0.1, 0.1, 0.1));
	/* nvgStroke(vg); */

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

	// render channels first
	for (i = 0; i < ln->channel_count; i++)
		draw_channel(vg, &ln->channels[i]);

	for (i = 0; i < ln->node_count; i++)
		draw_node(vg, &ln->nodes[i]);
}
