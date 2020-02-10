#include <stdint.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_main.h>
#include <SDL.h>
#include <curses.h>
#include <math.h>

struct Tyle {
  double elivation; //0 = deep ossen 1 = mounten .5 = sea leval
  double temperature; //0 = cold  1 = hot
  int64_t type;
};

enum colors {C_TEXT,C_OK,C_FAIL,C_HIGH,C_GRASS,C_STONE,C_MAGMA,C_WATER};

#include "font.c"
#include "env.h"
int64_t cursorX = 1 , cursorY = 1;
#include "config.c"
#include "map.c"
#include "raw.c"


//#define bool char

enum controls {EXIT = SDLK_q,UP=SDLK_w,DOWN=SDLK_s,RIGHT=SDLK_d,LEFT=SDLK_a,RESTART=SDLK_r,P_LAND=SDLK_z,P_WATER=SDLK_x,P_LAVA=SDLK_c,P_load=SDLK_o,P_save=SDLK_p};

int64_t last;

bool io(int64_t key,char S,struct Config data) {

  if (map==NULL) { //UMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    return 1;
  }

  switch (key) {
    case 0 :return 1;

    case EXIT:return 0;

    case LEFT:if(cursorX>0)cursorX-=1;return 1;
    case RIGHT:if(cursorX<MAPX-1)cursorX+=1;return 1;
    case UP:if(cursorY>0)cursorY-=1;return 1;
    case DOWN:if(cursorY<MAPY-1)cursorY+=1;return 1;

    case P_load:loadSave(data);return 1;
    case P_save:saveSave(data);return 1;

    case P_LAND:map[cursorX][cursorY].elivation = .51f;map[cursorX][cursorY].type = T_GRASS;map[cursorX][cursorY].temperature = 0.0f;return 1;
    case P_WATER:map[cursorX][cursorY].elivation = .0f;map[cursorX][cursorY].type  = T_STONE;map[cursorX][cursorY].temperature = 0.0f;return 1;
    case P_LAVA:map[cursorX][cursorY].temperature = gLavaPlaceTemp; map[cursorX][cursorY].elivation = 1;return 1;

  }

  return 1;
}

void game(SDL_RWops* configfile) {

  bool running = true;
  struct Config dataconfig;
  struct Raw rawdata;

  F_init();

  F_initpair(C_TEXT, 255,255,255, 0,0,0);

  F_initpair(C_OK,0,0,255, 0,0,0);
  F_initpair(C_FAIL, 255,0,0, 0,0,0);

  F_initpair(C_HIGH, 0x80,0,0x80, 0,0,0);
  F_initpair(C_WATER, 0,255,0, 0,0,0);
  F_initpair(C_MAGMA, 255,0,0, 0,0,0);
  F_initpair(C_GRASS, 0,0,255, 0,0,0);
  F_initpair(C_STONE, 255,255,255, 0,0,0);

  F_ATTR(F_COLOR_PAIR(C_TEXT));
  F_printw("[*] loading data");
  readconfig(configfile,&dataconfig);
  F_load(dataconfig.font);
  F_ATTR(F_COLOR_PAIR(C_OK));
  F_printw("\t[DONE]\n");
  F_ATTR(F_COLOR_PAIR(C_TEXT));
  F_printw("[*] loading objects...\n");
  readraw(dataconfig.rawfile,&rawdata);
  loadObj(&rawdata);
  SDL_Event e;

  F_ATTR(F_COLOR_PAIR(C_TEXT));
  F_printw("[*] making map");
  genaratemap();
  F_ATTR(F_COLOR_PAIR(C_OK));
  F_printw("\t\t[DONE]\n");
  F_ATTR(F_COLOR_PAIR(C_TEXT));
  F_printw("\nWorld sim; Press enter to start. nVerson : %s\n",gVerson);
  printw("X: %d Y: %d",(int)gWindowx,(int)gWindowy);
  F_getmaxxy(gWindowx,gWindowy);
  F_MVputch(0,0,'a');
  //mapw = newwin(SCRY+2,SCRX+2 ,1,1);
  F_more();
  //F_clear();
  //F_move(0,0);
  //F_refresh();


  int64_t last = clock();
  int64_t mspt = 0;
  frame = 0;

  cursorX = 0;
  cursorY = 0;

  //nonl();

  int key = 0;
  char shift = 0;

  //maprender(); 
  F_clear();
  while (running) {
    if (((CLOCKS_PER_SEC/1000)*gMSPT) <= (clock() - last)) {
      mspt = (((clock()-last)));
      last = clock();

      ticktyles();
      
      key = 0;

      while( SDL_PollEvent( &e ) != 0 ) {
        if( e.type == SDL_QUIT ) {
            running = false;
        } else if (e.type == SDL_KEYDOWN) {
            key = e.key.keysym.sym;
            shift = e.key.keysym.sym == KMOD_LSHIFT;
        }
      }

      running &= io(key,shift,dataconfig);
      //maprender();
      //F_move(0,0);
      //printw("fps: %d, x:  %d, y: %d   ",(1000/(mspt+1)),(int)cursorX,(int)cursorY);
      F_refresh();
      frame++;
    }
  }

  F_clear();
  F_refresh();
  F_end();
  //DOES NOT DEALOCK MAP & NMAP
  printf("By nVoidPointer (nvoidpointer@gmail.com) (buggybugs@catty)\n");
  return;
}
