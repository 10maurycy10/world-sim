#include "toolkit.h"

enum MENUE { MENUE_MAIN,
             MENUE_GAME,
             MENUE_EXIT };

void ioHandeler(int x) {
  printf("%c", x);
}

int mainMenue() { //if the state ends, this returns next state
  int nextState = MENUE_MAIN;
  void ioHandle(int x) {
    printf("key press in main %d\n",x);
    switch (x) {
      AUTO_CASE(SDLK_ESCAPE, nextState = MENUE_EXIT );
      AUTO_CASE('g', nextState = MENUE_GAME );
    }
  };

  while (nextState == MENUE_MAIN) {
    C_refresh();
    pollIo(&ioHandle);
  }
  return nextState;
}