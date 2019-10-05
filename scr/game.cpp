#include <curses.h>
#include <stdlib.h>
enum boutons {BOSS,EXIT,INV,UP,DOWN,RIGHT,LEFT,RESTART,DROP,DEBUG}; //the controls
constexpr int cont[10] = {13,113,105,104,106,108,107,114,100,99}; //the keys for the controls
const char* items[2] = {"(empty)","test   "};//the item names

#define INVT 10 //the invetory size
int inv[10] = {0,0,0,0,0,0,0,0,0,0};//the invetry itslef

const char* verson = "0.6";

int playerx = 2;//player posision
int playery = 2;

#define MAPY 20 //map size
#define MAPX 20

char rendermap[MAPY][MAPX+1] = { //map + fog of war
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    ",
  "                    "};


char map[MAPY][MAPX+1] = {//the map
  "XXXXXXXXXXXXXXXXXXXX",
  "X..................X",
  "X..................X",
  "X..................X",
  "X..................X",
  "XXXXXXXXX+XXXXXXXXXX",
  "        X#X         ",
  "XXXXXXXXX+XXXXXXXXXX",
  "X..................X",
  "X..................X",
  "X..................X",
  "X..................X",
  "X..................X",
  "XXXXXXXXX+XXXXXXXXXX",
  "        X#X         ",
  "XXXXXXXXX+XXXXXXXXXX",
  "X..................X",
  "X..................X",
  "X%.................X",
  "XXXXXXXXXXXXXXXXXXXX"};

  struct obj { //entaty data
  int id; /* 0: test/null  1 - 101: items 101 - 201: mosters/npcs*/
  int x;
  int y;
};
#define OBJ 20 //the entaty maz
struct obj* mapobj[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //pointers for the entitys
#define ROOM 3 //room count
int roomdata[3][4] = { //the room data
  {0 ,0 ,20,5},
  {0 ,7 ,20,16},
  {0,18 ,20,20}
};


void clearmap() {;} //WIP  fog the Screen



/******************************************************************************
Screan layout
1: meige
2: map      |>inv
3:          |
4:          |
5:          |
6:          |
7:          |
8:          |
9:          |
10:         |
11:
12:
13:
14:
15:
16:
17:
18:
19:
20:
21:

******************************************************************************/
int getRoomId(int x,int y) {
  for (int i = 0;i<ROOM;i++) {//for all rooms
  if ((roomdata[i][0]<=playerx) && (roomdata[i][1]<=playery)) {//if is in romm:
    if ((roomdata[i][2]>=playerx) && (roomdata[i][3])>=playery) {
        return i+1; //return the roomid
      }
    }
  }
  return 0; //if in no room then passinges
}

void msg(const char* a) { //print a msg
  move(0,0);
  printw(a);
}
void cleanln(int x) { //redray a line
  move(x,0);
  printw(rendermap[x-1]);

  for (int y=0;y<OBJ;y++) { //render entetys
    if (mapobj[y] == NULL) {;}else{
      if ((*(mapobj[y])).y == x) {
        switch ((*(mapobj[y])).id) {
          case 1:mvaddch(x,(*(mapobj[y])).x,73);break;
          case 0:mvaddch(x,(*(mapobj[y])).x,109);break;
        }
      }
    }
  }

  if (x==playery) {//draw player
    mvaddch(playery,playerx,'@');
  }
}

void clearmsg() {
  move(0,0);
  clrtoeol();
}

int addobj(struct obj* o) { //add an entey
  for(int x = 0;x<OBJ;x++) {
    if (mapobj[x] == NULL) {
      mapobj[x] = o;
      cleanln(o->y);
      return x;
    }
  }

  return -1;
}

obj* rmobj(int x) { //reomve an enty (not dealocate it)
  obj* y = mapobj[x];
  mapobj[x] = NULL;
  return y;
}

bool give(int id) {
  //msg("call:give");
  for (int x = 0;x<INVT;x++) {
    //msg("iteration:give");
    if (inv[x]==0) {
      inv[x] = id;
      return 0;
    }
  }

  return 1;
}

void render() {//redray hole Screan
  for(int x = 0;x<(MAPY+1);x++) {
    cleanln(x);

  }
}

void genaratemap() { //reset the map
  obj* o = (obj*)malloc(sizeof(obj));
  o->y = 2;
  o->x = 2;
  o->id = 1;

  addobj(o);
}

void movep(int x,int y);
void restart(bool a) { //reset it all
  movep(3,2);
  if (a) {
    for (int x = 0;x<INVT;x++) {
        inv[x] = 0;
      }
    }

  for (int x = 0;x<OBJ;x++) {
    free(mapobj[x]);
    mapobj[x] = NULL;
  }

  genaratemap();
  render();
}

void movep(int x,int y) { //move the player
  for (int z=0;z<OBJ;z++) {
    if (mapobj[z] == NULL) {;} else {
      if (((*(mapobj[z])).y == y)&&((*(mapobj[z])).x == x)) {
        switch ((*(mapobj[z])).id) {
          case 1:if(give(1)==0){free(rmobj(z));}break;
          case 0:break;
        }
      }
    }
  }

  int oy = playery;
  if (map[y-1][x]=='X') {rendermap[y-1][x] = map[y-1][x];cleanln(y);;return;} else{
    playerx = x;
    playery = y;
    mvaddch(y,x,'@');
  }

  if (map[y-1][x]=='%') {
    msg("next leaval");
    getch();
    clearmsg();
    restart(0);
    return;
  }

  rendermap[y-1][x] = map[y-1][x];
  cleanln(oy);
}

void inventory() {
  msg("you have : -more-");

  for (int y = 0;y<INVT;y++) {
    move(y+1,0);
    printw("-  %d : %s  -",y,items[inv[y]]);
  }

  getch();
  for (int y = 0;y<INVT;y++) {cleanln(y+1);}
  clearmsg();
}

void drop() {
  msg("drop what?");
  int id = getch()-48;
  clearmsg();

  if ((id>INVT)||(id<0)) {
    msg("that item dose not egist.");
    return;
  }

  if (inv[id] == 0) {
    msg("that slot is empty");
    return;
  }

  obj* o = (obj*)malloc(sizeof(obj));
  o -> x = playerx;
  o -> y = playery;
  o -> id = inv[id];

  if (addobj(o)==-1) {
    return;
  }

  inv[id] = 0;
  return;
}

bool mechanics(int key) {
  clearmsg();
  if (map==NULL) {
    msg("WHAT!");
  }

  switch (key) {
    case cont[EXIT]:return 0;
    case cont[INV]:inventory();return 1;

    case cont[UP]:movep(playerx+0,playery-1);return 1;
    case cont[DOWN]:movep(playerx+0,playery+1);return 1;
    case cont[LEFT]:movep(playerx-1,playery+0);return 1;
    case cont[RIGHT]:movep(playerx+1,playery+0);return 1;
    case cont[RESTART]:restart(1);return 1;
    case cont[DROP]:drop();return 1;
    case cont[DEBUG]:return 0;
  }

  msg("Unrecognized command.");
  return 1;
}

void game() {
  bool running = true;
  int key = 0;
  initscr();
  raw();
  noecho();
  curs_set(0);
  move(0,0);
  printw((char*)"Text only game. [by M.Z.] -more- \nVerson : %s",verson);
  refresh();
  getch();
  clear();

  restart(1);

  while (running) {
    key = getch();
    running = mechanics(key);
    mvprintw(1,0,"%d", getRoomId(playerx,playery));
    refresh();
  }

  clear();
  refresh();
  endwin();
  clear();
  printf("By nVoidPointer (nvoidpointer@gmail.com)\n");
  return;
}
