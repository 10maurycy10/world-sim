#include <SDL.h>

SDL_Window *gWindow;
SDL_Surface *gWindowSurface;

void C_init() {
  gWindow = SDL_CreateWindow("World of Doom", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
  gWindowSurface = SDL_GetWindowSurface(gWindow);
}

void pollIo(void (*callback)(int)) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        callback(event.key.keysym.sym);
        break;

      /* SDL_QUIT event (window close) */
      case SDL_QUIT:
        SDL_Quit();
        _Exit(0);
        break;

      case SDL_WINDOWEVENT_RESIZED:
        gWindowSurface = SDL_GetWindowSurface(gWindow);
      break;

      default:
        break;
    }
  }
}