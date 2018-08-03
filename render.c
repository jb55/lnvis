
#include "render.h"
#include <stdio.h>
#include "nanovg/nanovg.h"

void draw_node(NVGcontext *vg, struct node *node)
{
	const float r = 20.0;
	const float pos = 500.0;

	/* const float h = r; */
	/* const float w = r; */
	/* const float x = pos; */
	/* const float y = pos; */
	/* const float kr = (int)(h * 0.35f); */
	/* const float cy = y + (int)(h * 0.5f); */


	NVGpaint bg;

	nvgSave(vg);
	nvgTranslate(vg, pos, pos);

	NVGcolor node_color =
		nvgRGBAf(node->color.r, node->color.g, node->color.b,
			 node->color.a);

	NVGcolor blend =
		nvgRGBAf(0, 0, 0, 0.5);

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
	for (int i = 0; i < ln->node_count; i++)
		draw_node(ln->vg, &ln->nodes[i]);
}
