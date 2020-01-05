#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_main.h>

char* rfile;

#include "config.c"
#include "raw.c"

//#include <pthread.h>
//#define bool char


void renderline(int); //redraw a line
void movep(int x,int y);


#define DEBUGCOMP

#ifdef DEBUGCOP
void debug(char* a) {
  printf("%s\n",a);
}
#else
void debug(char* a) {;}
#endif

double sealeval; //todo add more
double lavatemp;
int grasregrow;
int MSPT;
int lavaPlaceTemp;




int curs_x = 1 , curs_y = 1;

enum controls {BOSS='\n',EXIT='q',UP='w',DOWN='s',RIGHT='d',LEFT='a',RESTART='r',HELP='`',P_LAND='x',P_WATER='z',P_LAVA='c'}; //the controls

enum colors {C_MSG,C_GRASS,C_WATER,C_LAVA,C_ROCK,C_SCORC};
enum icons {CH_GRASS='w',CH_WATER='X',CH_ROCK};

#define SEALEVAL sealeval // TURN INTO RAWFILE ENTRYS
#define HOTTEMP lavatemp

const char* gVerson = "0.9";

struct tyle {
  double high; //0 = deep ossen 1 = mounten .5 = sea leval
  double temp; //0 = cold  1 = hot
  int isrock;
};

#define MAPY 20 //map size
#define MAPX 60
struct tyle **map;
struct tyle **nmap;

void msg(const char* a) { //print a msg
  attron(COLOR_PAIR(C_MSG));
  move(0,0);
  printw(a);
  attroff(COLOR_PAIR(C_MSG));
}


void cleanln(int y) { //redray a line x: on screan pos
  if (y > 0) {
    renderline(y);
  }
}

void clearmsg() {
  move(0,0);
  clrtoeol();
}


void render() {//redray hole Screan
  for(int y = 0;y<MAPY;y++) {
    cleanln(y+1);
  }
}

void genaratemap() { //reset the map
 for (int y = 0;y<MAPY;y++) {
   for (int x = 0;x<MAPX;x++) {
     map[x][y].high = 0.0f;
     map[x][y].temp = 0.0f;
     map[x][y].isrock = false;
   }
 }
}


void restart(bool a) { //reset it all a:if tho reset inv

  genaratemap();
  render();
}
void dotyle(int x, int y,struct tyle* dest) {
  if (map[x][y].isrock) {
    map[x][y].isrock -= 1;
  }
  if (map[x][y].temp > HOTTEMP) {
    map[x][y].isrock = grasregrow;
  }
  if (y == 0 || y == (MAPY-1)) {
    dest -> high = map[x][y].high;
    dest -> temp = map[x][y].temp; 
    return;
  } 
  if (x == 0 || x == (MAPX-1)) {
    dest -> temp = map[x][y].temp;
    dest -> high = map[x][y].high;
    return;
  } 

  dest -> isrock = map[x][y].isrock;
  dest -> high = map[x][y].high;
  dest -> temp = ((map[x][y+1].temp + map[x][y-1].temp + map[x+1][y].temp + map[x-1][y].temp )/4 *.1) + (map[x][y].temp *.9f);
}


void ticktyles() {
  struct tyle** tmp;

  for (int x = 0;x<MAPX;x++) {
    for (int y = 0;y<MAPY;y++) {
      dotyle(x,y,&(nmap[x][y]));
    }
  }
  tmp = nmap;
  nmap = map;
  map = tmp;
}
int last;

void loadObj(struct Raw* data) {
  sealeval = data -> seaLeval;
  lavatemp = data -> stoneMelt;
  grasregrow = data -> grassRegrow;
  lavaPlaceTemp = data -> lava;
  lavatemp = .25;
  sealeval = .5;
  MSPT = data -> MSPT;
}

bool mechanics(int key) {
  if ((last+((CLOCKS_PER_SEC/1000)*MSPT))<(clock())) {
    last = clock();
    ticktyles();
    render();
    refresh();
  }
  //ticktyles();
  if (map==NULL) {
    msg("WHAT!");
  }

  switch (key) {
    case 0 :return 1;

    case EXIT:return 0;
    case RESTART:restart(1);return 1;

    case LEFT:curs_x-=1;return 1;
    case RIGHT:curs_x+=1;return 1;
    case UP:curs_y-=1;return 1;
    case DOWN:curs_y+=1;return 1;

    case P_LAND:map[curs_x][curs_y].high = .51f;map[curs_x][curs_y].isrock = false;map[curs_x][curs_y].temp = 0.0f;return 1;
    case P_WATER:map[curs_x][curs_y].high = .0f;map[curs_x][curs_y].isrock = false;map[curs_x][curs_y].temp = 0.0f;return 1;
    case P_LAVA:map[curs_x][curs_y].temp = lavaPlaceTemp;map[curs_x][curs_y].isrock = true;return 1;

    case HELP:;
  }

  //msg("Unrecognized command.");

  mvprintw(20,0,"%d",key);
  return 1;
}


void game(SDL_RWops* configfile , char* rawfile) {
  bool running = true;
  rfile = rawfile;
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
  int key = 0;
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
  render();


  init_pair(C_MSG, COLOR_WHITE, COLOR_BLACK);

  init_pair(C_LAVA, COLOR_RED, COLOR_RED);
  init_pair(C_WATER, COLOR_BLUE, COLOR_BLUE);
  init_pair(C_GRASS, COLOR_GREEN, COLOR_BLACK);
  init_pair(C_ROCK, COLOR_WHITE, COLOR_BLACK);
  init_pair(C_SCORC, COLOR_RED, COLOR_BLACK);


  restart(1);

  while (running) {
    render();
    key = getch();
    running = mechanics(key);
    //cleanln(2);
    //mvprintw(1,0,"%d",getRoomId(gPlayerx,gPlayery));
    refresh();
  }

  clear();
  refresh();
  endwin();
  clear();
  printf("By nVoidPointer (nvoidpointer@gmail.com) (buggybugs@kitty)\n");
  for (int i = 0;i<MAPX;i++) {
    free(map[i]);
  }
  for (int i = 0;i<MAPX;i++) {
    free(nmap[i]);
  }
  return;
}


void renderline(int y) {//render a line line:: pos on screan
  //mvprintw(0,0,"%d",getRoomId(gPlayerx,gPlayery));
  move(y,0);
  int attr = 0;
  for (int x = 0;x<MAPX;x++) {

    if ((y-1 == curs_y)&(x == curs_x)) {
      attr = A_BLINK;
    } else {
      attr = A_NORMAL;
    }

    if (map[x][y-1].high > SEALEVAL) {

      if (map[x][y-1].temp>HOTTEMP) {
        attr = attr | COLOR_PAIR(C_LAVA);
      } else { 
        if (map[x][y-1].isrock) {
          attr = attr | COLOR_PAIR(C_ROCK);
        } else {
          attr = attr | COLOR_PAIR(C_GRASS);
        }
      }

      mvaddch(y,x,CH_GRASS | attr);
    } else {
      if (map[x][y-1].temp>HOTTEMP) {
        attr = attr | COLOR_PAIR(C_ROCK);
      } else {
        attr = attr | COLOR_PAIR(C_WATER);
      }
      mvaddch(y,x,CH_WATER | attr);
    }
  }
}

