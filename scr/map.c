#pragma once

#define MATS 256
int MAT_STONE,
    MAT_GRASS,
    MAT_AIR;
int getMat(char *);

struct MAT {
  char matTexture[3];
  int matMelt;
  int matDecompTemp;
  int matDecompTo;
  int matCol[3];
  SDL_bool matVoid;
};

struct MAT gMats[256];

void loadMats() {
  MAT_STONE = getMat("STONE");
  MAT_GRASS = getMat("GRASS");
  MAT_AIR = getMat("AIR");
}

char *gGrass = ",'.`";
#define gGrasscount 4
struct Tyle {
  int32_t temperature; // 1000 = 0c  1100 = 100c
  uint16_t Lmat;
  uint16_t Fmat;
  SDL_bool discoverd;
  union LData {
    int16_t mosstimer;
  } LairData;
  //uint16_t Ftype;
  //uint16_t Fleval;
};

struct Save {
  char magic[5];
  int16_t X;
  int16_t Y;
  struct Tyle data;
};

#define SAVE_MAGIC ((char *)("ARMS"))

struct Tyle **map;
struct Tyle **nmap;


void reveleMap(int x,int y) {
  map[x][y].discoverd = 1;
  if (x > 0)
    map[x-1][y].discoverd = 1;
  if (x < gMapx - 1)
    map[x+1][y].discoverd = 1;
  if (y > 0)
    map[x][y-1].discoverd = 1;
  if (y < gMapy - 1)
    map[x][y+1].discoverd = 1;
}

void genaratemap(int seed) { //reset the map

  srand(seed);

  gMapy = 100;
  gMapx = 100;

  if (map) {
    for (int i = 0; i < MAPX; i++) {
      free(map[i]);
    }
    for (int i = 0; i < MAPX; i++) {
      free(nmap[i]);
    }
    free(nmap);
    free(map);
  }

  map = malloc(MAPX * (sizeof(void *)));
  nmap = malloc(MAPX * (sizeof(void *)));
  for (int i = 0; i < MAPX; i++) {
    map[i] = malloc((sizeof(struct Tyle)) * MAPY);
  }
  for (int i = 0; i < MAPX; i++) {
    nmap[i] = malloc((sizeof(struct Tyle)) * MAPY);
  }

  int ch[MAPX / 30 + 1][MAPY / 30 + 1];

  for (int i = 0; i < MAPX / 30 + 1; i++)
    for (int e = 0; e < MAPY / 30 + 1; e++) {
      ch[i][e] = rand() % 2;
    }

  for (int y = 0; y < MAPY; y++) {
    for (int x = 0; x < MAPX; x++) {
      map[x][y].discoverd = 0;
      if (0 < ch[x / 30][y / 30]) {
        map[x][y].Lmat = MAT_STONE;
        map[x][y].Fmat = MAT_STONE;
      } else {
        map[x][y].Lmat = MAT_AIR;
        map[x][y].Fmat = MAT_GRASS;
      }
    }
  }
  for (int y = 0; y < (MAPY); y++) {
    for (int x = 0; x < (MAPX); x++) {
      map[x][y].temperature = 1000;
      if (map[x][y].Lmat == MAT_STONE) {
        map[x][y].LairData.mosstimer = -1;
      } else if (map[x][y].Lmat == MAT_AIR) {
        reveleMap(x,y);
      }
    }
  }
}

void loadSave(struct Config data) {

  struct Save *s = malloc((data.savefile->size(data.savefile)) + 1);
  SDL_RWseek(data.savefile, 0, RW_SEEK_SET);
  data.savefile->read(data.savefile, s, 1, data.savefile->size(data.savefile)); //copy into mem

  if (map) { //free old map
    for (int i = 0; i < MAPX; i++) {
      free(map[i]);
    }
    for (int i = 0; i < MAPX; i++) {
      free(nmap[i]);
    }
    free(nmap);
    free(map);
  }

  if (!(s->magic[0] == SAVE_MAGIC[0] && s->magic[1] == SAVE_MAGIC[1] && s->magic[2] == SAVE_MAGIC[2] && s->magic[3] == SAVE_MAGIC[3]))
    return;
  gMapx = (s)->X;
  gMapy = (s)->Y;

  map = malloc(MAPX * (sizeof(void *)));
  nmap = malloc(MAPX * (sizeof(void *)));
  for (int i = 0; i < MAPX; i++) {
    map[i] = malloc((sizeof(struct Tyle)) * MAPY);
  }
  for (int i = 0; i < MAPX; i++) {
    nmap[i] = malloc((sizeof(struct Tyle)) * MAPY);
  }

  for (int y = 0; y < MAPY; y++) {
    for (int x = 0; x < MAPX; x++) {
      map[x][y] = OFFSET(s->data, x + y * MAPX);
    }
  }

  free(s);
}

void saveSave(struct Config data) {
  int numTyle = MAPX * MAPY;
  struct Save *save = malloc(sizeof(struct Save) - sizeof(struct Tyle) + sizeof(struct Tyle) * numTyle);
  strcpy(save->magic, SAVE_MAGIC);
  save->Y = MAPY;
  save->X = MAPX;

  for (int x = 0; x < MAPX; x++)
    for (int y = 0; y < MAPY; y++)
      OFFSET(save->data, x + y * MAPX) = map[x][y];

  SDL_RWseek(data.savefile, 0, RW_SEEK_SET);
  data.savefile->write(data.savefile, save, 1, sizeof(struct Save) - sizeof(struct Tyle) + sizeof(struct Tyle) * numTyle);

  free(save);
}


void dotyle(int x, int y, struct Tyle *dest) {
  (*dest) = map[x][y];
  if (y != 0 && y != (MAPY - 1)) {
    if (x != 0 && x != (MAPX - 1)) {
      dest->temperature = ((map[x][y + 1].temperature + map[x][y - 1].temperature + map[x + 1][y].temperature + map[x - 1][y].temperature) / 4 * .02) + (map[x][y].temperature * .98f);
    }
  }

  if (map[x][y].Lmat == MAT_STONE) {
    if (map[x][y].LairData.mosstimer > -1) {
      if (map[x][y].LairData.mosstimer > 0)
        dest->LairData.mosstimer--;
      if (map[x][y].LairData.mosstimer == 0)
        dest->Lmat = MAT_GRASS;
    }
  } else if (map[x][y].Fmat == MAT_GRASS) {
    if (map[x][y].temperature > gMats[MAT_GRASS].matDecompTemp) {
      dest->LairData.mosstimer = gGrasregrow;
    }
  }
  if (dest->temperature > gMats[dest->Lmat].matDecompTemp && gMats[dest->Lmat].matDecompTemp > -1) {
    dest->Lmat = gMats[dest->Lmat].matDecompTo;
  }
  if (dest->temperature > gMats[dest->Fmat].matDecompTemp && gMats[dest->Fmat].matDecompTemp > -1) {
    dest->Fmat = gMats[dest->Fmat].matDecompTo;
  }
}

void ticktyles() {

  for (int x = 0; x < MAPX; x++) {
    for (int y = 0; y < MAPY; y++) {
      dotyle(x, y, &(nmap[x][y]));
    }
  }

  struct Tyle **tmp;
  tmp = nmap;
  nmap = map;
  map = tmp;
}

void renderchar(int x, int y, int xpos, int ypos) { //render a x:x(map) y:y(map) pos: pos on screan
  int state;
  int attr;
  int tylech = 0;
  if (gMats[map[x][y].Lmat].matVoid) {
    state = map[x][y].temperature > gMats[map[x][y].Fmat].matMelt; //draw the floor
    attr = F_COLOR_PAIR(gMats[map[x][y].Fmat].matCol[state]);
    tylech = gMats[map[x][y].Fmat].matTexture[state];
    if (map[x][y].Fmat == MAT_GRASS) {
      tylech = gGrass[((y ^ (gGrasscount / 2)) ^ ((x + 1) ^ ((gGrasscount)))) % gGrasscount];
    }
  } else {
    state = map[x][y].temperature > gMats[map[x][y].Lmat].matMelt; //draw lair
    attr = F_COLOR_PAIR(gMats[map[x][y].Lmat].matCol[state]);
    tylech = gMats[map[x][y].Lmat].matTexture[state];
  }
  if ((y == cursorY) && (x == cursorX) && ((frame % 60) > 30) && !(menueState == MEN_MAIN)) {
    tylech = 'X';
    attr = (F_COLOR_PAIR(C_HIGH));
  } else if (!map[x][y].discoverd){
    tylech = ' ';
    attr = F_COLOR_PAIR(C_TEXT);
  }
  F_ATTR(attr);
  F_MVputch(xpos + 1, ypos + 1, tylech);
}

void maprender() {
  for (int32_t x = 0; x < SCRX; x++)
    for (int32_t y = 0; y < SCRY; y++) {
      F_ATTR(F_COLOR_PAIR(C_TEXT));
      if (((x + cursorX - SCRX / 2) < MAPX) && ((x + cursorX - SCRX / 2) > -1))
        if (((y + cursorY - SCRY / 2) < MAPY) && ((y + cursorY - SCRY / 2) > -1))
          renderchar(x + cursorX - SCRX / 2, y + cursorY - SCRY / 2, x, y);
        else
          F_MVputch(x + 1, y + 1, ' ');
      else
        F_MVputch(x + 1, y + 1, ' ');
    }
}