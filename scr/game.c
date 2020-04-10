#include <SDL.h>

int gameLoop() {  //if the state ends, this returns next state
  int nextState = MENUE_GAME;
  void ioHandle(int x,int mod) {
    switch (x) {
      AUTO_CASE(SDLK_ESCAPE, nextState = MENUE_MAIN );
    }
  };
  int p = 0;
  while (nextState == MENUE_GAME) {
    while (!((SDL_GetTicks()-p) >= 9) && !(SDL_GetTicks()<p)) { //if more than 9 ms passed or over flow detected
      SDL_Delay(0);
    }
    pollIo(&ioHandle);
    C_clear();
    C_box(screen,FRONT_COLORS_DIM);
    C_focus_title(screen);
    C_printf("FPS: %d",FRONT_COLORS_DIM,1,1000/(SDL_GetTicks()-p));
    p = SDL_GetTicks();
    C_focus(screen);
    drawMap(Cursor,screen);
    C_refresh();
  }
  return nextState;
}