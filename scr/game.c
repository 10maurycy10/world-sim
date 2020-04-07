#include <SDL.h>

int gameLoop() {  //if the state ends, this returns next state
  int nextState = MENUE_GAME;
  void ioHandle(int x) {
    switch (x) {
      AUTO_CASE(SDLK_ESCAPE, nextState = MENUE_MAIN );
    }
  };

  while (nextState == MENUE_GAME) {
    pollIo(&ioHandle);
    C_refresh();
  }
  return nextState;
}