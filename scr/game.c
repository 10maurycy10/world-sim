#include <stdint.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_main.h>
#include <SDL.h>
#include <curses.h>
#include <math.h>
#include "font.c"

struct Tyle {
  double elivation; //0 = deep ossen 1 = mounten .5 = sea leval
  double temperature; //0 = cold  1 = hot
  int64_t type;
};

enum colors {C_TEXT,C_OK,C_FAIL,C_HIGH,C_GRASS,C_STONE,C_MAGMA,C_WATER};

#include "env.h"
int64_t cursorX = 1 , cursorY = 1;
#include "config.c"
#include "map.c"
#include "raw.c"


//#define bool char

enum controls {BOSS='\n',EXIT='q',UP='w',DOWN='s',RIGHT='d',LEFT='a',RESTART='r',P_LAND='z',P_WATER='x',P_LAVA='c',P_load='L',P_save='S'};

int64_t last;

bool io(int64_t key,struct Config data) {

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

  init_pair(C_TEXT, COLOR_WHITE, COLOR_BLACK);

  init_pair(C_OK, COLOR_GREEN, COLOR_BLACK);
  init_pair(C_FAIL, COLOR_RED, COLOR_BLACK);

  init_pair(C_HIGH, COLOR_YELLOW, COLOR_BLACK);
  init_pair(C_WATER, COLOR_BLUE, COLOR_BLACK);
  init_pair(C_MAGMA, COLOR_RED, COLOR_BLACK);
  init_pair(C_GRASS, COLOR_GREEN, COLOR_BLACK);
  init_pair(C_STONE, COLOR_WHITE, COLOR_BLACK);

  attrset(COLOR_PAIR(C_TEXT));
  printw("[*] loading data");
  readconfig(configfile,&dataconfig);
  attrset(COLOR_PAIR(C_OK));
  printw("\t[DONE]\n");
  attrset(COLOR_PAIR(C_TEXT));
  printw("[*] loading objects...\n");
  attrset(A_DIM);
  readraw(dataconfig.rawfile,&rawdata);
  loadObj(&rawdata);

  attrset(COLOR_PAIR(C_TEXT));
  printw("[*] making map");
  genaratemap();
  attrset(COLOR_PAIR(C_OK));
  printw("\t\t[DONE]\n");
  attrset(COLOR_PAIR(C_TEXT));
  printw("\nWorld sim; Press enter to start. nVerson : %s\n",gVerson);
  getch();
  clear();
  getmaxyx(stdscr,gWindowy,gWindowx);
  mapw = newwin(SCRY+2,SCRX+2 ,1,1);
  noecho();
  curs_set(0);
  move(0,0);
  refresh();
  //clear();
  nodelay(stdscr , TRUE);


  int64_t last = clock();
  int64_t mspt = 0;
  frame = 0;

  cursorX = 0;
  cursorY = 0;

  //nonl();


  maprender(); 
  while (running) {
    if (((CLOCKS_PER_SEC/1000)*gMSPT) < (clock()-last)) {
      mspt = (((clock()-last)));
      last = clock();

      ticktyles();

      running = io(getch(),dataconfig);
      maprender();
      move(0,0);
      printw("fps: %d, x: %d, y: %d   ",(1000/(mspt+1)),cursorX,cursorY);
      refresh();
      frame++;
    }
  }

  clear();
  refresh();
  endwin();
  clear();
  //DOES NOT DEALOCK MAP & NMAP
  printf("By nVoidPointer (nvoidpointer@gmail.com) (buggybugs@catty)\n");
  return;
}
