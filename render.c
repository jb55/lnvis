
#include "render.h"
#include <stdio.h>
#include "nanovg/nanovg.h"

void draw_node(NVGcontext *vg, struct node *node)
{
	const float r = 50.0;
	const float pos = 100.0;

	const float h = r;
	const float w = r;
	const float x = pos;
	const float y = pos;
	const float kr = (int)(h * 25.0f);
	const float cy = y + (int)(h * 5.0f);

	NVGpaint bg;

	nvgSave(vg);

	bg = nvgRadialGradient(vg, x + (int)(pos * w), cy + 1, kr - 3, kr + 3,
			       nvgRGBA(0, 0, 0, 64), nvgRGBA(0, 0, 0, 0));
	nvgBeginPath(vg);
	nvgCircle(vg, pos, pos, r);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	nvgText(vg, x + r / 2.0, y + r + 5.0, node->alias, NULL);
	nvgRestore(vg);
}

void render_ln(struct ln *ln)
{
	for (int i = 0; i < ln->node_count; i++)
		draw_node(ln->vg, &ln->nodes[i]);
}
