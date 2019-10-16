#include <curses.h>
#include <stdlib.h>



#define DEBUGCOMP

#ifdef DEBUGCOP
void debug(char* a) {
  printf("%s\n",a);
}
#else
void debug(char* a) {;}
#endif





enum boutons {BOSS,EXIT,INV,UP,DOWN,RIGHT,LEFT,RESTART,DROP,DEBUG}; //the controls
constexpr int gCont[10] = {13,113,105,104,106,108,107,114,100,99}; //the keys for the controls
const char* gItems[2] = {"(empty)","test   "};//the item names

#define INVT 10 //the invetory size
int inv[10] = {0,0,0,0,0,0,0,0,0,0};//the invetry itslef

const char* gVerson = "0.7";

int gPlayerx = 2;//player posision
int gPlayery = 1;
int gRoom = 0;

#define MAPY 29 //map size
#define MAPX 26

char rendermap[MAPY][MAPX+2] = { //map + fog of war
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  ".........................."
};



char map[MAPY][MAPX+2] = {//the map
  "XXXXXXX...XXXXXXXX........",
  "X.....X...X......X...XXXXX",
  "X.....XXXXX......X...X...X",
  "X.....+###+......XXXXX...X",
  "X.....XXXXX......+###+...X",
  "XXX+XXX...X......XXXXX+XXX",
  "..X#X.....XXXXX+XX....#...",
  "..X#X.................#...",
  "..X#X.....................",
  ".XX+XXXXX.................",
  ".X......X.................",
  ".X......X.................",
  ".X......X.................",
  ".XXXXXXXX.................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "..........................",
  "........XXXX+XXX..........",
  "...........X#X............",
  "..........XX+XXX..........",
  "..........X....XXXXXX#X...",
  "..........X....+######X...",
  "..........X....XXXXXXXX...",
  "..........X....X..........",
  "..........X....X..........",
  "..........X....X..........",
  "..........XXXXXX..........",
  };

  struct obj { //entaty data
  int id; /* 0: test/null  1 - 101: gItems 101 - 201: mosters/npcs*/
  int x;
  int y;
};
#define OBJ 20 //the entaty maz
struct obj* mapobj[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //pointers for the entitys
#define ROOM 4 //room count
int roomdata[ROOM][4] = { //the room data
  {0 ,0 ,6 ,5 },  //test room
  {10,0 ,17,6 },
  {21,1 ,25,5 },
  {1 ,9 ,8 ,13}
};


void fillroom(int);



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


void clearmap() {//reset the fog of doooooooooooooooooooomm
  for (int y = 0;y<(MAPY);y++) {
    for (int x = 0;x<(MAPX);x++) {
      rendermap[y][x] = 0x20;
    }
    rendermap[y][MAPX] = 0x00;
  }
}

int getRoomId(int x,int y) {

  for (int i = 0;i<ROOM;i++) {//for all rooms
  if ((roomdata[i][0]<=(gPlayerx)) && (roomdata[i][1]<=(gPlayery))) {//if is in romm:
    if ((roomdata[i][2]>=gPlayerx) && (roomdata[i][3])>=gPlayery) {
        //mvprintw(0,0,"%d",i+1);
        return i+1; //return the roomid
      }
    }
  }
  //mvprintw(0,0,"0");
  return 0; //if in no room then passinges
}


void msg(const char* a) { //print a msg
  move(0,0);
  printw(a);
}

void renderline(char*,int); //redraw a line

void cleanln(int y) { //redray a line x: on screan pos
  move(y,0);
  if (y > 0) {
    renderline(rendermap[y-1],y);
  }

  for (int ypos=0;ypos<OBJ;ypos++) { //render entetys
    if (mapobj[ypos] == NULL) {;}else{
      if ((*(mapobj[ypos])).y+1 == ypos) {
        if (getRoomId((*(mapobj[ypos])).x,(*(mapobj[ypos])).y)==gRoom){
          switch ((*(mapobj[ypos])).id) {
            case 1:mvaddch(ypos,(*(mapobj[ypos])).x,73);break;
            case 0:mvaddch(ypos,(*(mapobj[ypos])).x,109);break;
          }
        }
      }
    }
  }

  if (y-1==gPlayery) {//draw player
    mvaddch(y,gPlayerx,'@');
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
      cleanln(o->y+1);
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
  for(int x = 0;x<(MAPY);x++) {
    cleanln(x+1);
  }
}

void genaratemap() { //reset the map
  for (int x = 0;x<OBJ;x++) {
    free(mapobj[x]);
    mapobj[x] = NULL;
  }

  obj* o = (obj*)malloc(sizeof(obj));
  o->y = 2;
  o->x = 2;
  o->id = 1;

  addobj(o);
}

void movep(int x,int y);

void restart(bool a) { //reset it all a:if tho reset inv
  if (a) {
    for (int x = 0;x<INVT;x++) {
        inv[x] = 0;
      }
    }



  movep(3,2);
  genaratemap();
  clearmap();
  fillroom(getRoomId(gPlayerx,gPlayery));
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

  int oy = gPlayery;
  if (map[y][x]=='X') {
    cleanln(y+1);
    return;
  } else {
    gPlayerx = x;
    gPlayery = y;
    cleanln(y+1);
  }
  fillroom(gRoom);
  gRoom = getRoomId(gPlayerx,gPlayery);
  fillroom(gRoom);
  if (map[y][x]=='%') {
    msg("next leaval");
    cleanln(oy+1);
    getch();
    clearmsg();
    restart(0);
    return;
  }
  cleanln(y+1);
  cleanln(oy+1);
}

void inventory() {
  msg("you have : -more-");
  for (int y = 0;y<INVT;y++) {
    move(y+1,0);
    printw("-  %d : %s  -",y,gItems[inv[y]]);
  }
  getch();
  for (int y = 0;y<INVT;y++) {
    cleanln(y+1);
  }
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
  o -> x = gPlayerx;
  o -> y = gPlayery;
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
    case gCont[EXIT]:return 0;
    case gCont[INV]:inventory();return 1;

    case gCont[UP]:movep(gPlayerx+0,gPlayery-1);return 1;
    case gCont[DOWN]:movep(gPlayerx+0,gPlayery+1);return 1;
    case gCont[LEFT]:movep(gPlayerx-1,gPlayery+0);return 1;
    case gCont[RIGHT]:movep(gPlayerx+1,gPlayery+0);return 1;
    case gCont[RESTART]:restart(1);return 1;
    case gCont[DROP]:drop();return 1;
    case gCont[DEBUG]:return 0;
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
  printw("Text only game. [by M.Z.] -more- \ngVerson : %s",gVerson);
  refresh();
  getch();
  clear();
  render();

  restart(1);

  while (running) {
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
  printf("By nVoidPointer (nvoidpointer@gmail.com)\n");
  return;
}

void fillroom(int room) { //fill a room room:roomid
  int index = room - 1;
  //mvprintw(0,0,"%d",room);
  if (room==0) { //if it is a passing
    rendermap[gPlayery][gPlayerx] = map[gPlayery][gPlayerx]; //the player pos

    if (map[gPlayery-1][gPlayerx] == '#' || map[gPlayery-1][gPlayerx] == '+' || map[gPlayery-1][gPlayerx] == '.') {
      rendermap[gPlayery-1][gPlayerx] = map[gPlayery-1][gPlayerx];
    } //all agationt points
    if (map[gPlayery+1][gPlayerx] == '#' || map[gPlayery+1][gPlayerx] == '+'|| map[gPlayery+1][gPlayerx] == '.') {
      rendermap[gPlayery+1][gPlayerx] = map[gPlayery+1][gPlayerx];
    }
    if (map[gPlayery][gPlayerx+1] == '#' || map[gPlayery][gPlayerx+1] == '+' || map[gPlayery][gPlayerx+1] == '.') {
      rendermap[gPlayery][gPlayerx+1] = map[gPlayery][gPlayerx+1];
    }
    if (map[gPlayery][gPlayerx-1] == '#' || map[gPlayery][gPlayerx-1] == '+' || map[gPlayery][gPlayerx-1] == '.') {
      rendermap[gPlayery][gPlayerx-1] = map[gPlayery][gPlayerx-1];
    }

    cleanln(gPlayery+1); //redraw it all
    cleanln(gPlayery+2);
    cleanln(gPlayery);
    return;
  } else {

    for (int chy = roomdata[index][1];roomdata[index][3]>=chy;chy++) { //else for all og the room
      for (int chx = roomdata[index][0];roomdata[index][2]>=chx;chx++) {
        if (getRoomId(chx,chy)==gRoom) {
          rendermap[chy][chx] = map[chy][chx];
        } else {
          if (map[chy][chx]=='.') {
            rendermap[chy][chx] = ' ';
          } else {
            rendermap[chy][chx] = map[chy][chx];
          }
        }
      }
      cleanln(chy+1); //redraw the line
    }
  }
}

void renderline(char* line,int y) {//render a line line: text y: pos on screan
  mvprintw(0,0,"%d",getRoomId(gPlayerx,gPlayery));
  move(y,0);
  for (int x = 0;x<MAPX;x++) {
    mvaddch(y,x,line[x]);
  }
}
