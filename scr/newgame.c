#include "toolkit.h"
#include <SDL.h>

int newGame() {
  int nextState = MENUE_NEW_GAME;
  loadMatRaw();
  void ioHandle(int x, int mod) {
    switch (x) {
      AUTO_CASE(SDLK_ESCAPE, nextState = MENUE_MAIN);
      AUTO_CASE(SDLK_RETURN,
        generatemap();
        nextState = MENUE_GAME;
      )
    }
  };

  while (nextState == MENUE_NEW_GAME) {
    pollIo(&ioHandle);
    C_clear();
    C_box(screen, FRONT_COLORS_DIM);
    C_focus_title(screen);
    C_printf("[New Game]", FRONT_COLORS_RED, 0);
    C_focus(screen);
    C_printf("\nEsc: back\n", FRONT_COLORS_TEXT, 0);
    C_printf("Return: generate new map\n", FRONT_COLORS_TEXT, 0);
    C_refresh();
  }
  return nextState;
}