#include "tyle.c"
#include "save.c"

struct IntVect2 mapSize = {0,0};
struct Tyle **map = NULL;
struct Tyle **nmap = NULL;

void ticktyles() {

  for (int x = 0; x < mapSize.x; x++) {
    for (int y = 0; y < mapSize.y; y++) {
      dotyle(x, y, &(nmap[x][y]),map);
    }
  }

  struct Tyle **tmp;
  tmp = nmap;
  nmap = map;
  map = tmp;
};

void freeMap(struct Tyle *** data,struct IntVect2 size) {
  if (*data) {
    for (int i = 0;i < size.x; i++) {
      //printf("%p\n",(*data)[i]);
      free((*data)[i]);
    }
    free(*data);
  }
  *data = NULL;
}

struct Tyle **allocMap() {
  struct Tyle ** data;
  data = malloc(mapSize.x * sizeof(void *));
  for (int i = 0; i < mapSize.x; i++) {
    data[i] = malloc(mapSize.y * sizeof(struct Tyle));
  }
  return data;
}

void generatemap() {

  haveMap = 1;

  freeMap(&map,mapSize);
  freeMap(&nmap,mapSize);

  int seed = SDL_GetTicks(); //milisecons since start ;)
  srand(seed);

  Cursor.x = 0;
  Cursor.y = 0;

  mapSize.x = 100;
  mapSize.y = 100;

  nmap = allocMap();
  map = allocMap();

  for (int x = 0; x < mapSize.x; x++)
    for(int y = 0; y < mapSize.y; y++) {
      map[x][y].temperature = 1020;
      map[x][y].Lmat = MAT_GRASS;
    }

}

void drawMap(struct IntVect2 cursor, struct Viewport box) {
  for (int sx = box.tl.x + 1; sx < box.br.x; sx++) //for intire viewport
    for (int sy = box.tl.y + 1; sy < box.br.y; sy++) {
      int mx = sx-box.tl.x - box.br.x / 2 + Cursor.x;
      int my = sy-box.tl.y - box.br.y / 2 + Cursor.y;

      if (mx > -1 && mx < mapSize.x && my > -1 && my < mapSize.y) {
        drawTyle(sx, sy, map[mx][my]); //draw tile
        if (mx == Cursor.x && my == Cursor.y) {
          C_mvaddch(sx, sy, 'X', encodeC(3,3,0,0,0,0));
        }
      }
    }
}
