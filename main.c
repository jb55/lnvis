#include "gl.h"

#include <stdio.h>
#include <time.h>
#ifdef NANOVG_GLEW
#include <GL/glew.h>
#endif
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include "nanovg/nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg/nanovg_gl.h"

#include "defs.h"
#include "perf.h"
#include "demo.h"
#include "render.h"
#include "update.h"
#include "ln.h"

void errorcb(int error, const char *desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

int premult = 0;

static void key(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	/* if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) */
	/* 	blowup = !blowup; */
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		premult = !premult;
}

static struct ln ln;
static double mx, my;
static int mdown = 0;
static int mclicked = 0;

static const union color dark_color = {
	.rgba = { 0x28 / 255.0, 0x2c / 255.0, 0x34 / 255.0, 1.0f }
};

static const union color light_color = {
	.rgba = { 1.0, 1.0, 1.0, 1.0f }
};


void mouse_pos(GLFWwindow *win, double x, double y)
{
	(void)win;
	mx = x;
	my = y;
}

void mouse_click(GLFWwindow *win, int button, int action, int mods)
{
	(void)win;
	(void)button;
	(void)action;
	(void)mods;

	mclicked = action == 1 && button == 0;

	if (button == 0)
		mdown = action;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)mods;
	(void)scancode;
	(void)window;

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_A:
			ln.display_flags ^= DISP_ALIASES;
			break;
		case GLFW_KEY_S:
			ln.display_flags ^= DISP_STROKE_NODES;
			break;
		case GLFW_KEY_G:
			ln.display_flags ^= DISP_GRID;
			break;
		case GLFW_KEY_T:
			if (ln.display_flags & DISP_DARK)
				memcpy(&ln.clear_color, &light_color, sizeof(ln.clear_color));
			else
				memcpy(&ln.clear_color, &dark_color, sizeof(ln.clear_color));
			ln.display_flags ^= DISP_DARK;
			break;
		}
	}
}

int main()
{
	GLFWwindow *window;
	int first = 1;
	DemoData data;
	NVGcontext *vg = NULL;
	PerfGraph fps;
	double prevt = 0;

	srand(time(0));
	// ln collision grid subdivision
	// cells = grid_div * grid_div
	static const int grid_div = 20;
	static const int dark_theme = 1;

	u64 flags = DISP_DARK
		  /* | DISP_ALIASES */
		  | DISP_GRID
		  /* | DISP_STROKE_NODES */
		  ;

	ln.display_flags = flags;

	if (dark_theme)
		memcpy(&ln.clear_color, &dark_color, sizeof(ln.clear_color));
	else
		memcpy(&ln.clear_color, &light_color, sizeof(ln.clear_color));

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}

	initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");

	glfwSetErrorCallback(errorcb);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#ifdef DEMO_MSAA
	glfwWindowHint(GLFW_SAMPLES, 4);
#endif

	window = glfwCreateWindow(1000, 600, "Lightning Network Vis", NULL, NULL);
	//	window = glfwCreateWindow(1000, 600, "NanoVG", glfwGetPrimaryMonitor(), NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key);

	glfwMakeContextCurrent(window);
#ifdef NANOVG_GLEW
	if (glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
#endif

#ifdef DEMO_MSAA
	vg = nvgCreateGL2(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

	ln.vg = vg;
	init_ln(&ln, grid_div);

	if (loadDemoData(vg, &data) == -1)
		return -1;

	glfwSwapInterval(1);

	glfwSetTime(0);
	prevt = glfwGetTime();

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_click);
	glfwSetCursorPosCallback(window, mouse_pos);

	while (!glfwWindowShouldClose(window)) {
		if (ln.clicked)
			ln.clicked = 0;
		double t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;

		t = glfwGetTime();
		dt = t - prevt;
		prevt = t;
		updateGraph(&fps, dt);

		ln.clicked = mclicked;
		mclicked = 0;

		ln.mx = mx;
		ln.my = my;
		ln.mdown = mdown;

		glfwGetCursorPos(window, &mx, &my);
		glfwGetWindowSize(window, &winWidth, &winHeight);

		ln.window_height = winHeight;
		ln.window_width = winWidth;

		if (first) {
			random_network(winWidth, winHeight, 3, 500, &ln);
			printf("channels %d\n", ln.channel_count);
			first = 0;
		}

		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

		// Calculate pixel ration for hi-dpi devices.
		pxRatio = (float)fbWidth / (float)winWidth;

		// Update and render
		glViewport(0, 0, fbWidth, fbHeight);

		glClearColor(ln.clear_color.r,
			     ln.clear_color.g,
			     ln.clear_color.b,
			     ln.clear_color.a);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

		/* renderDemo(vg, mx, my, winWidth, winHeight, t, 1, &data); */
		update(&ln, dt);
		render_ln(&ln);
		renderGraph(vg, 5, 5, &fps);

		nvgEndFrame(vg);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	free_ln(&ln);
	freeDemoData(vg, &data);

	nvgDeleteGL2(vg);

	glfwTerminate();
	return 0;
}
