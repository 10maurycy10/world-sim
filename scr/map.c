#ifndef false
#define false 0
#define true 1
#endif

#define MATS 256
int MAT_STONE,
    MAT_GRASS,
    MAT_UNDISCOVERED;
int getMat(char *);

void loadMats() {
  MAT_STONE = getMat("STONE");
  MAT_GRASS = getMat("GRASS");
  MAT_UNDISCOVERED = getMat("UNDISCOVERED");
}

char matTexture[MATS][3] = {
    {'#', '~', '*'},
    {' ', '*', '*'},
    {'*', '*', '*'}, //curanty ignored
};
int matMelt[MATS] = {
    2000,
    2000,
    2000,
};
int matDecompTemp[MATS] = {
    -1,
    -1,
    1150,
};
int matDecompTo[MATS] = {
    -1,
    -1,
    -1,
};
int matCol[MATS][3] = {
    {C_TEXT, C_MAGMA, C_TEXT},
    {C_TEXT, C_TEXT, C_TEXT},
    {C_GRASS, C_TEXT, C_TEXT},
};
char *gGrass = ",'.`";
#define gGrasscount 4
struct Tyle {
  int32_t temperature; // 1000 = 0c  1100 = 100c
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
      map[x][y].temperature = 1000;
      if (0 < ch[x / 30][y / 30]) {
        map[x][y].Lmat = MAT_STONE;
        map[x][y].LairData.mosstimer = -1;
      } else
        map[x][y].Lmat = MAT_GRASS;
    }
  }
  free(ch);
  for (int y = 1; y < (MAPY - 1); y++) {
    for (int x = 1; x < (MAPX - 1); x++) {
      if (map[x][y].Lmat == MAT_STONE) {
        map[x][y].LairData.mosstimer = -1;
        if ((map[x - 1][y].Lmat == MAT_GRASS) || (map[x + 1][y].Lmat == MAT_GRASS) || (map[x][y + 1].Lmat == MAT_GRASS) || (map[x][y - 1].Lmat == MAT_GRASS))
          map[x][y].Lmat = MAT_STONE;
        else
          map[x][y].Lmat = MAT_UNDISCOVERED;
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
  } else if (map[x][y].Lmat == MAT_GRASS) {
    if (map[x][y].temperature > matDecompTemp[MAT_GRASS]) {
      dest->LairData.mosstimer = gGrasregrow;
    }
  }
  if (dest->temperature > matDecompTemp[dest->Lmat] && matDecompTemp[dest->Lmat] > -1) {
    dest->Lmat = matDecompTo[dest->Lmat];
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
  int state = map[x][y].temperature > matMelt[map[x][y].Lmat];
  int attr = F_COLOR_PAIR(matCol[map[x][y].Lmat][state]);
  int tylech = 0;
  tylech = matTexture[map[x][y].Lmat][state];
  if (map[x][y].Lmat == MAT_GRASS) {
    tylech = gGrass[((y ^ (gGrasscount / 2)) ^ ((x + 1) ^ ((gGrasscount)))) % gGrasscount];
  }
  if ((y == cursorY) && (x == cursorX) && ((frame % 60) > 30) && !(menueState == MEN_MAIN)) {
    tylech = 'X';
    attr = (F_COLOR_PAIR(C_HIGH));
  } else {
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