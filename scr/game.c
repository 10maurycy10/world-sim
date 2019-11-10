#include <curses.h>
#include <stdlib.h>

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

int curs_x = 1 , curs_y = 1;

enum boutons {BOSS='\n',EXIT='q',UP='w',DOWN='s',RIGHT='d',LEFT='a',RESTART='r',DEBUG='`',P_LAND='x',P_WATER='z',P_LAVA='c'}; //the controls

enum colors {C_MSG,C_GRASS,C_WATER,C_LAVA};
enum icons {GRASS='w',WATER='~'};
#define SEALEVAL 0.5f

const char* gVerson = "0.8";

struct tyle {
  double high; //0 = deep ossen 1 = mounten .5 = sea leval
  double temp; //0 = cold  1 = hot
};

#define MAPY 100 //map size
#define MAPX 100
struct tyle map[MAPY][MAPX];

void msg(const char* a) { //print a msg
  attron(COLOR_PAIR(C_MSG));
  move(0,0);
  printw(a);
  attroff(COLOR_PAIR(C_MSG));
}


void cleanln(int y) { //redray a line x: on screan pos
  renderline(y);
}

void clearmsg() {
  move(0,0);
  clrtoeol();
}


void render() {//redray hole Screan
  for(int x = 0;x<(MAPY);x++) {
    cleanln(x+1);
  }
}

void genaratemap() { //reset the map
 for (int y = 0;y<MAPY;y++) {
   for (int x = 0;x<MAPX;x++) {
     map[x][y].high = 0.0f;
   }
 }
}


void restart(bool a) { //reset it all a:if tho reset inv
 
  genaratemap();
  render();
}



bool mechanics(int key) {
  clearmsg();
  if (map==NULL) {
    msg("WHAT!");
  }

  switch (key) {
    case EXIT:return 0;
    case RESTART:restart(1);return 1;

    case LEFT:curs_x-=1;return 1;
    case RIGHT:curs_x+=1;return 1;
    case UP:curs_y-=1;return 1;
    case DOWN:curs_y+=1;return 1;

    case P_LAND:map[curs_x][curs_y].high = .51f;return 1;
    case P_WATER:map[curs_x][curs_y].high = .0f;return 1;
    case P_LAVA:map[curs_x][curs_y].temp = 1.0f;return 1;

    case DEBUG:;return 1;
  }

  msg("Unrecognized command.");
  mvprintw(20,0,"%d",key);
  return 1;
}

void game() {
  bool running = true;
  int key = 0;
  initscr();
  start_color();
  raw();
  noecho();
  curs_set(0);
  move(0,0);
  printw("Text only game. [by M.Z.] -more- \nVerson : %s",gVerson);
  refresh();
  getch();
  clear();
  render();

  init_pair(C_MSG, COLOR_WHITE, COLOR_BLACK);

  init_pair(C_LAVA, COLOR_RED, COLOR_BLACK);
  init_pair(C_WATER, COLOR_BLUE, COLOR_GRAYTEXT);
  init_pair(C_GRASS, COLOR_GREEN, COLOR_BLACK);


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
  return;
}


void renderline(int y) {//render a line line: text y: pos on screan
  //mvprintw(0,0,"%d",getRoomId(gPlayerx,gPlayery));
  move(y,0);
  int attr = 0;
  for (int x = 0;x<MAPX;x++) {

    if ((y == curs_y)&(x == curs_x)) {
      attr = A_BLINK;
    } else {
      attr = A_NORMAL;
    }

    if (map[x][y].high > SEALEVAL) {
      attr = attr | COLOR_PAIR(C_GRASS);
      mvaddch(y,x,GRASS | attr);
    } else {
      attr = attr | COLOR_PAIR(C_WATER);
      mvaddch(y,x,WATER | attr);
    }
  }
}

