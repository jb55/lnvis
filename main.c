
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <assert.h>

#include "defs.h"

struct ln {
	int quitting;
};

void process_events(struct ln *ln, const u8 *keys) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if (keys[SDL_SCANCODE_Q] || keys[SDL_SCANCODE_ESCAPE])
				ln->quitting = 1;
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				//handle_resize(camera, event.window.data1, event.window.data2);
				break;
			}
			break;
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
		}
	}
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
	ln.quitting = 0;

	// nanovg needs this
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 );

	SDL_Window *window =
		SDL_CreateWindow("Lightning Network Visualizer", 0, 0, width,
				 height,
				 SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	SDL_GL_CreateContext(window);

	u32 last = SDL_GetTicks();
	int nkeys;

	const u8 *keys = SDL_GetKeyboardState(&nkeys);
	assert(nkeys > 0);

	while (1) {
		process_events(&ln, keys);
		if (ln.quitting)
			break;

		u32 ticks = SDL_GetTicks();
		update(&ln);

		last = ticks;
		(void)last;
		render(&ln);

		/* Swap front and back buffers */
		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();

	return 0;
}
