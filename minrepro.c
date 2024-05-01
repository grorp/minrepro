#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>

#define INTERVAL_MS 5000
bool relative = true;

Uint32 timer_callback(Uint32 interval, void* param)
{
	if (relative) {
		SDL_SetRelativeMouseMode(SDL_FALSE);
		relative = false;
		printf("\n[Relative mouse mode disabled]\n\n");
	} else {
		SDL_SetRelativeMouseMode(SDL_TRUE);
		relative = true;
		printf("\n[Relative mouse mode enabled]\n\n");
	}
	return INTERVAL_MS;
}

int main(int argc, char* argv[])
{
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
	SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		printf("SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(
			"Minrepro",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			800, 600,
			0);
	if (!window) {
		printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_TimerID timer_id = SDL_AddTimer(INTERVAL_MS, timer_callback, NULL);
	if (timer_id == 0) {
		printf("SDL_AddTimer failed: %s\n", SDL_GetError());
		return 1;
	}
	timer_callback(INTERVAL_MS, NULL);

	bool quit = false;

	while (!quit) {
		SDL_Event e;
		SDL_WaitEvent(&e);

		int w = 0, h = 0;
		SDL_GetWindowSize(window, &w, &h);

		switch (e.type) {
		case SDL_MOUSEMOTION:
			printf("SDL_MOUSEMOTION (t=%d)\n", e.common.timestamp);
			printf("	which=%i (SDL_TOUCH_MOUSEID=%i)\n", e.motion.which, SDL_TOUCH_MOUSEID);
			printf("	x=%i y=%i\n", e.motion.x, e.motion.y);
			break;
		case SDL_MOUSEBUTTONDOWN:
			printf("SDL_MOUSEBUTTONDOWN (t=%d)\n", e.common.timestamp);
			printf("	which=%i (SDL_TOUCH_MOUSEID=%i)\n", e.button.which, SDL_TOUCH_MOUSEID);
			printf("	x=%i y=%i\n", e.button.x, e.button.y);
			printf("	button=%i\n", e.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			printf("SDL_MOUSEBUTTONUP (t=%d)\n", e.common.timestamp);
			printf("	which=%i (SDL_TOUCH_MOUSEID=%i)\n", e.button.which, SDL_TOUCH_MOUSEID);
			printf("	x=%i y=%i\n", e.button.x, e.button.y);
			printf("	button=%i\n", e.button.button);
			break;
		case SDL_FINGERMOTION:
			printf("SDL_FINGERMOTION (t=%d)\n", e.common.timestamp);
			printf("	touchId=%i fingerId=%i\n", e.tfinger.touchId, e.tfinger.fingerId);
			printf("	x=%i y=%i\n", (int)(e.tfinger.x * w), (int)(e.tfinger.y * h));
			break;
		case SDL_FINGERDOWN:
			printf("SDL_FINGERDOWN (t=%d)\n", e.common.timestamp);
			printf("	touchId=%i fingerId=%i\n", e.tfinger.touchId, e.tfinger.fingerId);
			printf("	x=%i y=%i\n", (int)(e.tfinger.x * w), (int)(e.tfinger.y * h));
			break;
		case SDL_FINGERUP:
			printf("SDL_FINGERUP (t=%d)\n", e.common.timestamp);
			printf("	touchId=%i fingerId=%i\n", e.tfinger.touchId, e.tfinger.fingerId);
			printf("	x=%i y=%i\n", (int)(e.tfinger.x * w), (int)(e.tfinger.y * h));
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE)
				quit = true;
			break;
		case SDL_QUIT:
			quit = true;
			break;
		default:
			break;
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_RemoveTimer(timer_id);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

