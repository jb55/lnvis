
#include <SDL2/SDL.h>
#include <GL/gl.h>

#include "defs.h"

struct ln {
};

static void process_events()
{
	//const u8 *keystates = SDL_GetKeyboardState(NULL);
}

static void update(struct ln *ln)
{
	(void)ln;
}

static void render(struct ln *ln)
{
	(void)ln;
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	int width = 640;
	int height = 480;
	struct ln ln;

	SDL_Window *window =
		SDL_CreateWindow("Lightning Network Visualizer", 0, 0, width,
				 height,
				 SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	SDL_GL_CreateContext(window);

	u32 last = SDL_GetTicks();

	while (1) {
		process_events();
		u32 ticks = SDL_GetTicks();
		update(&ln);

		last = ticks;
		(void)last;
		render(&ln);

		/* Swap front and back buffers */
		SDL_GL_SwapWindow(window);
	}

	return 0;
}
