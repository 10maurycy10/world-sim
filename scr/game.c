#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

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

int curs_x = 1 , curs_y = 1;

enum controls {BOSS='\n',EXIT='q',UP='w',DOWN='s',RIGHT='d',LEFT='a',RESTART='r',HELP='`',P_LAND='x',P_WATER='z',P_LAVA='c'}; //the controls

enum colors {C_MSG,C_GRASS,C_WATER,C_LAVA,C_ROCK,C_SCORC};
enum icons {CH_GRASS='w',CH_WATER='~',CH_ROCK};

#define SEALEVAL 0.5f
#define HOTTEMP 0.1f

const char* gVerson = "0.8";

struct tyle {
  double high; //0 = deep ossen 1 = mounten .5 = sea leval
  double temp; //0 = cold  1 = hot
  bool isscorched;
};

#define MAPY 30 //map size
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
     map[x][y].isscorched = false;
   }
 }
}


void restart(bool a) { //reset it all a:if tho reset inv

  genaratemap();
  render();
}
void dotyle(int x, int y,struct tyle* dest) {
  if (map[x][y].isscorched) {
    map[x][y].isscorched = false;
  }
  if (map[x][y].temp > HOTTEMP) {
    map[x][y].isscorched = true;
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
  dest -> high = map[x][y].high;
  dest -> temp = ((map[x][y+1].temp + map[x][y-1].temp + map[x+1][y].temp + map[x-0][y].temp )/4 *.1) + (map[x][y].temp *.9f);
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
bool mechanics(int key) {
  clearmsg();
  if ((last+(CLOCKS_PER_SEC/2))<(clock())) {
    last = clock();
    ticktyles();
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

    case P_LAND:map[curs_x][curs_y].high = .51f;return 1;
    case P_WATER:map[curs_x][curs_y].high = .0f;return 1;
    case P_LAVA:map[curs_x][curs_y].temp = 1.0f;return 1;

    case HELP:
      move(0,0);
      printw(".-help---------------,\n");
      printw("|%c this help         |\n",HELP);
      printw("|w,a,s,d move cursor |\n");
      printw("|%c place grass       |\n",P_LAND);
      printw("|%c place water       |\n",P_WATER);
      printw("|%c place LAVA        |\n",P_LAVA);
      printw("`--------------------'\n");

      printw("\n");
      nodelay(stdscr,false);
      getch();
      nodelay(stdscr,false);
      clearmsg();
      return 1;
  }

  msg("Unrecognized command.");
  mvprintw(20,0,"%d",key);
  return 1;
}

void game() {
  bool running = true;
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
  nodelay(stdscr, TRUE);
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
        if (map[x][y-1].isscorched) {
          attr = attr | COLOR_PAIR(C_SCORC);
        } else {
          attr = attr | COLOR_PAIR(C_GRASS);
        }
      }

      mvaddch(y,x,CH_GRASS | attr);
    } else {
      if (map[x][y-1].temp>HOTTEMP) {
        attr = attr | COLOR_PAIR(C_SCORC);
      } else {
        attr = attr | COLOR_PAIR(C_WATER);
      }
      mvaddch(y,x,CH_WATER | attr);
    }
  }
}

