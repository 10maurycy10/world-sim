#include "toolkit.h"

bool haveMap = 0;

enum MENUE { MENUE_MAIN,
             MENUE_GAME,
             MENUE_EXIT,
             MENUE_NEW_GAME };

void ioHandeler(int x) {
  printf("%c", x);
}

int mainMenue() { //if the state ends, this returns next state

  int nextState = MENUE_MAIN;
  int selecton = 0;

  void drawOpton(int x, char* text) {
    int attr = FRONT_COLORS_DIM;
    if (x == selecton) {
      attr = FRONT_COLORS_TEXT;
    }
    C_puts(text,attr);
  };

  void ioHandle(int x, int mod) {
    switch (x) {
      AUTO_CASE(SDLK_UP, selecton-- );
      AUTO_CASE(SDLK_DOWN, selecton++ );
      AUTO_CASE(SDLK_RETURN, switch (selecton) {
        AUTO_CASE(0,nextState = MENUE_NEW_GAME);
        AUTO_CASE(1,nextState = MENUE_GAME)
        AUTO_CASE(3,nextState = MENUE_EXIT);
      })
    }
    selecton = SDL_max(selecton,0);
    selecton = SDL_min(selecton,3);
  };

  while (nextState == MENUE_MAIN) {

    struct Viewport center = C_mkPort(screen.br.x / 2 - 10,screen.tl.y,screen.br.x,screen.br.y);
    C_clear();
    C_focus(center);
    C_box(screen,FRONT_COLORS_DIM);
    C_printf("%s",FRONT_COLORS_TEXT,1,Null);
    C_printf("\n\n\t\t\tWorld of Doom\n",FRONT_COLORS_RED,0);
    C_printf("\t\t\tverson: %d.%d-%d\n",FRONT_COLORS_DIM,3,0,0,4);

    C_puts("\n\n\n",0);

    drawOpton(0,"\t\t\tplay new game\n");
    if (haveMap) //ARGGGGGGGGGGGGGGGGGGG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      drawOpton(1,"\t\t\tcontinue\n");
    else
      C_puts("\t\t\tcontinue\n",selecton == 1 ?encodeC(3,0,0,0,0,0) : encodeC(1,1,1,0,0,0));
    if (0)
      drawOpton(2,"\t\t\tnew world\n");
    else
      C_puts("\t\t\tnew world\n",selecton == 2 ?encodeC(3,0,0,0,0,0) : encodeC(1,1,1,0,0,0));
    drawOpton(3,"\t\t\texit\n");

    C_refresh();
    pollIo(&ioHandle);
  }
  return nextState;
}