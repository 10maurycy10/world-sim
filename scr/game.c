#include <SDL.h>

int gameLoop() {  //if the state ends, this returns next state

  struct Viewport mapb = {{0,0},{0,0}};
  struct Viewport helpScreen;
  enum {NONE,LOOK,PLACE};
  int render = NONE;
  int nextState = MENUE_GAME;
  void ioHandle(int x,int mod) {
    bool isfloor = 0;
    switch (x) {
      AUTO_CASE(SDLK_ESCAPE, if (render == NONE)  nextState = MENUE_MAIN; else render = NONE );
      AUTO_CASE(SDLK_k, render = LOOK );
      AUTO_CASE(SDLK_b, render = PLACE );
      AUTO_CASE(SDLK_UP, Cursor.y--; );
      AUTO_CASE(SDLK_DOWN, Cursor.y++; );
      AUTO_CASE(SDLK_LEFT, Cursor.x--; );
      AUTO_CASE(SDLK_RIGHT, Cursor.x++; );
    }
    Cursor.x = SDL_min(SDL_max(Cursor.x,0),mapSize.x - 1);
    Cursor.y = SDL_min(SDL_max(Cursor.y,0),mapSize.y - 1);
  };
  int p = 0;
  while (nextState == MENUE_GAME) {
    while (!((SDL_GetTicks()-p) >= 9) && !(SDL_GetTicks()<p)) { //if more than 9 ms passed or over flow detected
      //SDL_Delay(0);
    }

    mapb.br.x = screen.br.x-21;
    mapb.br.y = screen.br.y;

    helpScreen.tl.x = mapb.br.x;
    helpScreen.tl.y = 0;
    helpScreen.br = screen.br;

    pollIo(&ioHandle);
    C_clear();
    C_box(mapb,FRONT_COLORS_DIM);
    C_focus_title(mapb);
    C_printf("FPS: %d",FRONT_COLORS_DIM,1,1000/(SDL_GetTicks()-p));
    p = SDL_GetTicks();
    C_focus(mapb);
    drawMap(Cursor,mapb);

    C_box(helpScreen,FRONT_COLORS_DIM);
    C_focus(helpScreen);
    switch (render) {
      case NONE:
        C_printf(" Esc : exit\n",FRONT_COLORS_TEXT,0);
        C_printf("K : examine\n",FRONT_COLORS_TEXT,0);
        break;
      case LOOK:
        C_printf(" Esc : exit\n",FRONT_COLORS_TEXT,0);
        C_printf("\nLOOK\n",FRONT_COLORS_TEXT,0);
        C_puts(gMats[map[Cursor.x][Cursor.y].Lmat].desc,gMats[map[Cursor.x][Cursor.y].Lmat].nameCol);
        C_puts("\n",FRONT_COLORS_TEXT);
        break;
      case PLACE:
        C_printf(" Esc : exit\n",FRONT_COLORS_TEXT,0);
        C_printf("\nbuild\n",FRONT_COLORS_TEXT,0);
        break;

    }
    C_printf("",FRONT_COLORS_TEXT,0);
    ticktyles();

    C_refresh();
  }
  return nextState;
}