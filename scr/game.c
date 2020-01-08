#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_main.h>
#include <SDL.h>
#include <curses.h>


#include "env.h"

#include "config.c"
#include "raw.c"

//#define bool char

int cursorX = 1 , cursorY = 1;

enum controls {BOSS='\n',EXIT='q',UP='w',DOWN='s',RIGHT='d',LEFT='a',RESTART='r',P_LAND='z',P_WATER='x',P_LAVA='c'};
enum colors {C_TEXT,C_GRASS,C_STONE,C_MAGMA};
enum icons {CH_STONE =  '#',CH_WATER = '~',CH_GRASS = 'W'};

struct tyle {
  double high; //0 = deep ossen 1 = mounten .5 = sea leval
  double temp; //0 = cold  1 = hot
  int isrock;
};


struct tyle **map;
struct tyle **nmap;

void genaratemap() { //reset the map
 for (int y = 0;y<MAPY;y++) {
   for (int x = 0;x<MAPX;x++) {
     map[x][y].high = 0.0f;
     map[x][y].temp = 0.0f;
     map[x][y].isrock = false;
   }
 }
}

int last;

void loadObj(struct Raw* data) {
  gSealeval = data -> seaLeval;
  gLavatemp = data -> stoneMelt;
  gGrasregrow = data -> grassRegrow;
  gLavaPlaceTemp = data -> lava;
  gMSPT = data -> MSPT;
}

bool io(int key) {
  //ticktyles();
  if (map==NULL) {
    return 1;
  }

  switch (key) {
    case 0 :return 1;

    case EXIT:return 0;

    case LEFT:cursorX-=1;return 1;
    case RIGHT:cursorX+=1;return 1;
    case UP:cursorY-=1;return 1;
    case DOWN:cursorY+=1;return 1;

    case P_LAND:map[cursorX][cursorY].high = .51f;map[cursorX][cursorY].isrock = false;map[cursorX][cursorY].temp = 0.0f;return 1;
    case P_WATER:map[cursorX][cursorY].high = .0f;map[cursorX][cursorY].isrock = false;map[cursorX][cursorY].temp = 0.0f;return 1;
    case P_LAVA:map[cursorX][cursorY].temp = gLavaPlaceTemp;map[cursorX][cursorY].isrock = true;return 1;

  }

  return 1;
}



int frame = 0;

void renderchar(int x,int y,int xpos,int ypos) {//render a line line:: pos on screan
  //mvprintw(0,0,"%d",getRoomId(gPlayerx,gPlayery));
    int attr;
    char tylech = 'X';
    if ((y == cursorY)&&(x == cursorX))  {
      attr = A_BLINK;
    } else {
      attr = A_NORMAL;
    }

    /*if (map[x][y].high > gSealeval) {

      if (map[x][y].temp>gLavatemp) {
        //attr = attr | COLOR_PAIR(C_LAVA);
      } else { 
        if (map[x][y].isrock) {
          //attr = attr | COLOR_PAIR(C_ROCK);
        } else {
          //attr = attr | COLOR_PAIR(C_GRASS);
        }
      }

      mvaddch(y,x,CH_GRASS | attr);
    } else {
      if (map[x][y].temp>gLavatemp) {
        //attr = attr | COLOR_PAIR(C_ROCK);
      } else {
        //attr = attr | COLOR_PAIR(C_WATER);
      }
    }*/
    mvaddch(ypos,xpos,tylech | attr);
}

void maprender() {
  for (int x = 0;x<SCRX;x++) {
    for (int y = 0;y<SCRY;y++) {
      if ((x-cursorX)<MAPX && (x-cursorX)>-1)
        if ((y-cursorY<MAPY) && (y+SCRY/2-cursorY)>-1)
          renderchar(x-cursorX+SCRX/2,y-cursorY+SCRY/2,x,y+1);
        else
        mvaddch(y+1,x,' ');
      else
        mvaddch(y+1,x,' ');
    }
  }
  refresh();
}

void game(SDL_RWops* configfile) {

  bool running = true;
  struct Config config;
  struct Raw rawdata;

  readconfig(configfile,&config);
  readraw(config.rawfile,&rawdata);
  loadObj(&rawdata);
  map = malloc(MAPX*(sizeof(void*)));
  nmap = malloc(MAPX*(sizeof(void*)));
  for (int i = 0;i<MAPX;i++) {
    map[i] = malloc((sizeof(struct tyle))*MAPY);
  }
  for (int i = 0;i<MAPX;i++) {
    nmap[i] = malloc((sizeof(struct tyle))*MAPY);
  }
  last = clock();
  initscr();
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

  cursorX = 2;
  cursorY = 2;


  maprender();

  init_pair(C_TEXT, COLOR_WHITE, COLOR_BLACK);
  //init_pair(C_, COLOR_RED, COLOR_RED);
  //init_pair(CH_WATER, COLOR_BLUE, COLOR_BLUE);
  //init_pair(C_GRASS, COLOR_GREEN, COLOR_BLACK);
  //init_pair(C_ROCK, COLOR_WHITE, COLOR_BLACK);
  //init_pair(C_SCORC, COLOR_RED, COLOR_BLACK);

  maprender(); 
  while (running) {
    if ((last+((CLOCKS_PER_SEC/1000)*gMSPT))<(clock())) {
      last = clock();
      //ticktyles();

      running = io(getch());
      maprender();
      frame++;
    }
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


