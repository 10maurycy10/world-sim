#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_main.h>
#include <SDL.h>
#include <curses.h>
#include <math.h>

#include "env.h"
#include "config.c"
#include "raw.c"
int cursorX = 1 , cursorY = 1;
#include "map.c"

//#define bool char

enum controls {BOSS='\n',EXIT='q',UP='w',DOWN='s',RIGHT='d',LEFT='a',RESTART='r',P_LAND='z',P_WATER='x',P_LAVA='c'};

int last;

bool io(int key) {
  //ticktyles();
  if (map==NULL) { //UMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    return 1;
  }

  switch (key) {
    case 0 :return 1;

    case EXIT:return 0;

    case LEFT:if(cursorX>0)cursorX-=1;return 1;
    case RIGHT:if(cursorX<MAPX)cursorX+=1;return 1;
    case UP:if(cursorY>0)cursorY-=1;return 1;
    case DOWN:if(cursorY<MAPY)cursorY+=1;return 1;

    case P_LAND:map[cursorX][cursorY].elivation = .51f;map[cursorX][cursorY].type = T_GRASS;map[cursorX][cursorY].temperature = 0.0f;return 1;
    case P_WATER:map[cursorX][cursorY].elivation = .0f;map[cursorX][cursorY].type  = T_STONE;map[cursorX][cursorY].temperature = 0.0f;return 1;
    case P_LAVA:map[cursorX][cursorY].temperature = gLavaPlaceTemp;map[cursorX][cursorY].elivation = 1;return 1;

  }

  return 1;
}

void game(SDL_RWops* configfile) {

  bool running = true;
  struct Config config;
  struct Raw rawdata;

  initscr();
  readconfig(configfile,&config);
  readraw(config.rawfile,&rawdata);
  loadObj(&rawdata);
  mapw = newwin(SCRY+2,SCRX+2 ,1,1);
  map = malloc(MAPX*(sizeof(void*)));
  nmap = malloc(MAPX*(sizeof(void*)));
  for (int i = 0;i<MAPX;i++) {
    map[i] = malloc((sizeof(struct tyle))*MAPY);
  }
  for (int i = 0;i<MAPX;i++) {
    nmap[i] = malloc((sizeof(struct tyle))*MAPY);
  }
  last = clock();
  start_color();
  raw();
  noecho();
  curs_set(0);
  printw("World sim; ` for help -more- \nVerson : %s",gVerson);
  move(0,0);
  refresh();
  getch();
  clear();
  nodelay(stdscr , TRUE);

  genaratemap();

  int fps = 0;
  frame = 0;
  cursorX = 0;
  cursorY = 0;



  init_pair(C_TEXT, COLOR_WHITE, COLOR_BLACK);
  init_pair(C_HIGH, COLOR_YELLOW, COLOR_BLACK);

  init_pair(C_WATER, COLOR_BLUE, COLOR_BLACK);
  init_pair(C_MAGMA, COLOR_BLUE, COLOR_BLACK);
  init_pair(C_GRASS, COLOR_GREEN, COLOR_BLACK);
  init_pair(C_STONE, COLOR_WHITE, COLOR_BLACK);


  maprender(); 
  while (running) {
    if ((last+((CLOCKS_PER_SEC/1000)*gMSPT))<(clock())) {
      fps = ((1000/((clock()-last)*(CLOCKS_PER_SEC/1000)))+fps )/2;
      last = clock();

      running = io(getch());
      maprender();
      frame++;
      move(0,0);
      printw("%d , x: %d , y: %d    ",fps,cursorX,cursorY);
      refresh();
    }
    #ifndef LOL
    // if LOL is defined use a while(1) {} wating loop
    if ((Uint32)(((CLOCKS_PER_SEC/1000)*gMSPT - (last - clock())) < ((CLOCKS_PER_SEC/1000)*gMSPT))) {
      SDL_Delay(((CLOCKS_PER_SEC/1000*gMSPT - (last - clock()))));
    }
    #endif
  }

  clear();
  refresh();
  endwin();
  clear();
  printf("By nVoidPointer (nvoidpointer@gmail.com) (buggybugs@catty)\n");
  for (int i = 0;i<MAPX;i++) {
    free(map[i]);
  }
  for (int i = 0;i<MAPX;i++) {
    free(nmap[i]);
  }
  return;
}
