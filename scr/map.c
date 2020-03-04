#ifndef false
#define false 0
#define true 1
#endif

#define MATS 256
enum MAT { MAT_WALL,
         MAT_GRASS,
         MAT_STONE,
};

char matTexture[MATS][3] = {
  {' ','*','*'},
  {'*','*','*'}, //curanty ignored
  {'#','~','*'}
};
int matMelt[MATS] = {
  2000,
  2000,
  2000,
};
int matDecompTemp[MATS] = {
  -1,
  1150,
  -1,
};
int matDecompTo[MATS] = {
  -1,
  MAT_STONE,
  -1
};
int matCol[MATS][3] = {
  {C_TEXT,C_TEXT,C_TEXT},
  {C_GRASS,C_TEXT,C_TEXT},
  {C_TEXT,C_MAGMA,C_TEXT},
};
char *gGrass = ",'.`";
#define gGrasscount 4
struct Tyle {
  uint32_t temperature; // 1000 = 0c  1100 = 100c
  uint16_t Lmat;
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

void genaratemap(int seed) { //reset the map

  srand(seed);

  gMapy = 100;
  gMapx = 100;

  if (map) {
    for (int64_t i = 0; i < MAPX; i++) {
      free(map[i]);
    }
    for (int64_t i = 0; i < MAPX; i++) {
      free(nmap[i]);
    }
    free(nmap);
    free(map);
  }

  map = malloc(MAPX * (sizeof(void *)));
  nmap = malloc(MAPX * (sizeof(void *)));
  for (int64_t i = 0; i < MAPX; i++) {
    map[i] = malloc((sizeof(struct Tyle)) * MAPY);
  }
  for (int64_t i = 0; i < MAPX; i++) {
    nmap[i] = malloc((sizeof(struct Tyle)) * MAPY);
  }

  int ch[MAPX / 30 + 1][MAPY / 30 + 1];

  for (int i = 0; i < MAPX / 30 + 1; i++)
    for (int e = 0; e < MAPY / 30 + 1; e++) {
      ch[i][e] = rand() % 2;
    }

  int distancefrom00 = 0;
  int distancefrom01 = 0;
  int distancefrom11 = 0;
  int distancefrom10 = 0;

  for (int64_t y = 0; y < MAPY; y++) {
    for (int64_t x = 0; x < MAPX; x++) {
      map[x][y].temperature = 1000;
      distancefrom00 = x % 30 + y % 30;
      distancefrom01 = x % 30 + 30 - ((y) % 30);
      distancefrom10 = 30 - ((x) % 30) + y % 30;
      distancefrom11 = 30 - ((x) % 30) + 30 - ((y) % 30);
      int effect00 = ch[x / 30][y / 30] * 40 / (distancefrom00 + 1),
          effect01 = ch[x / 30 + 1][y / 30] * 40 / (distancefrom01 + 1),
          effect10 = ch[x / 30][y / 30 + 1] * 40 / (distancefrom10 + 1),
          effect11 = ch[x / 30 + 1][y / 30 + 1] * 40 / (distancefrom11 + 1);
      if (0 < ch[x / 30][y / 30])
        map[x][y].Lmat = MAT_STONE;
      else
        map[x][y].Lmat = MAT_GRASS;
    }
  }
  free(ch);
  for (int64_t y = 1; y < (MAPY - 1); y++) {
    for (int64_t x = 1; x < (MAPX - 1); x++) {
      if (map[x][y].Lmat == MAT_STONE) {
        map[x][y].LairData.mosstimer = -1;
        if ((map[x - 1][y].Lmat == MAT_GRASS) || (map[x + 1][y].Lmat == MAT_GRASS) || (map[x][y + 1].Lmat == MAT_GRASS) || (map[x][y - 1].Lmat == MAT_GRASS))
          map[x][y].Lmat = MAT_STONE;
        else
          map[x][y].Lmat = MAT_WALL;
      }
    }
  }
}

void loadSave(struct Config data) {

  struct Save *s = malloc((data.savefile->size(data.savefile)) + 1);
  SDL_RWseek(data.savefile, 0, RW_SEEK_SET);
  data.savefile->read(data.savefile, s, 1, data.savefile->size(data.savefile)); //copy into mem

  if (map) { //free old map
    for (int64_t i = 0; i < MAPX; i++) {
      free(map[i]);
    }
    for (int64_t i = 0; i < MAPX; i++) {
      free(nmap[i]);
    }
    free(nmap);
    free(map);
  }

  if (!(s->magic[0] == SAVE_MAGIC[0] && s->magic[1] == SAVE_MAGIC[1] && s->magic[2] == SAVE_MAGIC[2] && s->magic[3] == SAVE_MAGIC[3]))

    gMapx = (s)->X;
  gMapy = (s)->Y;

  map = malloc(MAPX * (sizeof(void *)));
  nmap = malloc(MAPX * (sizeof(void *)));
  for (int64_t i = 0; i < MAPX; i++) {
    map[i] = malloc((sizeof(struct Tyle)) * MAPY);
  }
  for (int64_t i = 0; i < MAPX; i++) {
    nmap[i] = malloc((sizeof(struct Tyle)) * MAPY);
  }

  for (int64_t y = 0; y < MAPY; y++) {
    for (int64_t x = 0; x < MAPX; x++) {
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

void dotyle(int64_t x, int64_t y, struct Tyle *dest) {
  (*dest) = map[x][y];
  if (y == 0 || y == (MAPY - 1)) {
    return;
  }
  if (x == 0 || x == (MAPX - 1)) {
    return;
  }
  dest->temperature = ((map[x][y + 1].temperature + map[x][y - 1].temperature + map[x + 1][y].temperature + map[x - 1][y].temperature) / 4 * .02) + (map[x][y].temperature * .98f);


  switch (map[x][y].Lmat) {
    case MAT_STONE:
      if (map[x][y].LairData.mosstimer > -1) {
        if (map[x][y].LairData.mosstimer > 0)
          dest->LairData.mosstimer--;
        if (map[x][y].LairData.mosstimer == 0)
          dest->Lmat = MAT_GRASS;
      }
      break;
    case MAT_GRASS:
      if (map[x][y].temperature > matDecompTemp[MAT_GRASS]) {
        dest->LairData.mosstimer = gGrasregrow;
      }
      break;
  }
  if (dest->temperature > matDecompTemp[dest->Lmat] && matDecompTemp[dest->Lmat] > -1) {
    dest->Lmat = matDecompTo[dest->Lmat];
  }
}

void ticktyles() {

  for (int64_t x = 0; x < MAPX; x++) {
    for (int64_t y = 0; y < MAPY; y++) {
      dotyle(x, y, &(nmap[x][y]));
    }
  }

  struct Tyle **tmp;
  tmp = nmap;
  nmap = map;
  map = tmp;
}

void renderchar(int64_t x, int64_t y, int64_t xpos, int64_t ypos) { //render a x:x(map) y:y(map) pos: pos on screan
  int state = map[x][y].temperature > matMelt[map[x][y].Lmat];
  int64_t attr = F_COLOR_PAIR(matCol[map[x][y].Lmat][state]);
  int64_t tylech = 0;
  tylech = matTexture[map[x][y].Lmat][state];
  switch (map[x][y].Lmat) {
    case MAT_GRASS:
      tylech = gGrass[((y ^ (gGrasscount / 2)) ^ ((x + 1) ^ ((gGrasscount)))) % gGrasscount];
      break;
  }
  if ((y == cursorY) && (x == cursorX) && ((frame % 60) > 30)) {
    tylech = 'X';
    attr = (F_COLOR_PAIR(C_HIGH));
  } else {
  }
  F_ATTR(attr);
  F_MVputch(xpos + 1, ypos + 1, tylech);
}

void maprender() {
  for (int32_t x = 0; x < SCRX; x++)
    for (int32_t y = 0; y < SCRY; y++)
      if (((x + cursorX - SCRX / 2) < MAPX) && ((x + cursorX - SCRX / 2) > -1))
        if (((y + cursorY - SCRY / 2) < MAPY) && ((y + cursorY - SCRY / 2) > -1))
          renderchar(x + cursorX - SCRX / 2, y + cursorY - SCRY / 2, x, y);
        else
          F_MVputch(x + 1, y + 1, ' ');
      else
        F_MVputch(x + 1, y + 1, ' ');
}