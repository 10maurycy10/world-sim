#ifndef false
#define false 0
#define true 1
#endif

enum S { T_STONE,
         T_GRASS };

void genaratemap() { //reset the map

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

  for (int64_t y = 0; y < MAPY; y++) {
    for (int64_t x = 0; x < MAPX; x++) {
      map[x][y].elivation = 0.5f;
      map[x][y].temperature = 0.0f;
      map[x][y].type = T_GRASS;
    }
  }
  return;
}

enum icons { CH_STONE = '#',
             CH_WATER = '~' };
char *gGrass = ",'.`";
#define gGrasscount 4

void loadSave(struct Config data) {

  if (gSfile)
    free(gSfile);

  gSfile = malloc((data.savefile->size(data.savefile)) + 1);

  data.savefile->read(data.savefile, gSfile, 1, data.savefile->size(data.savefile));

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

  gMapy = ((struct Save *)gSfile)->Y;
  gMapx = ((struct Save *)gSfile)->X;

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
      map[x][y].elivation = 0.5f;
      map[x][y].temperature = 0.0f;
      map[x][y].type = T_GRASS;
    }
  }
}

void saveSave(struct Config data) {
  struct Save *save = malloc(sizeof(struct Save) - 1);
  save->Y = gMapy;
  save->X = gMapy;

  data.savefile->write(data.savefile, save, 1, sizeof(struct Save) - 1);

  free(save);
}

void dotyle(int64_t x, int64_t y, struct Tyle *dest) {

  if (y == 0 || y == (MAPY - 1)) {
    dest->elivation = map[x][y].elivation;
    dest->temperature = map[x][y].temperature;
    dest->type = map[x][y].type;
    return;
  }
  if (x == 0 || x == (MAPX - 1)) {
    dest->elivation = map[x][y].elivation;
    dest->temperature = map[x][y].temperature;
    dest->type = map[x][y].type;
    return;
  }

  dest->type = map[x][y].type;
  dest->elivation = map[x][y].elivation;
  dest->temperature = ((map[x][y + 1].temperature + map[x][y - 1].temperature + map[x + 1][y].temperature + map[x - 1][y].temperature) / 4 * .02) + (map[x][y].temperature * .98f);
  switch (map[x][y].type) {
  case T_STONE:
    if (map[x][y].data.mosstimer)
      dest->data.mosstimer--;
    if (map[x][y].data.mosstimer < 0)
      dest->type = T_GRASS;
    break;
  case T_GRASS:
    if (map[x][y].temperature > gLavatemp) {
      dest->type = T_STONE;
      dest->data.mosstimer = gGrasregrow;
    }
    break;
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
  int64_t attr = 0;
  int64_t tylech = 0;
  switch (map[x][y].type) {
  case T_GRASS:
    attr |= F_COLOR_PAIR(C_GRASS);
    tylech |= gGrass[((y ^ (gGrasscount / 2)) ^ ((x + 1) ^ ((gGrasscount)))) % gGrasscount];
    break;
  case T_STONE:
    if (map[x][y].temperature > gLavatemp) {
      attr |= F_COLOR_PAIR(C_MAGMA);
      tylech |= CH_WATER;
    } else {
      attr |= F_COLOR_PAIR(C_STONE);
      tylech |= CH_STONE;
    }

    break;
  default:
    tylech = 'X';
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
  for (int64_t x = 0; x < SCRX; x++)
    for (int64_t y = 0; y < SCRY; y++)
      if (((x + cursorX - SCRX / 2) < MAPX) && ((x + cursorX - SCRX / 2) > -1))
        if (((y + cursorY - SCRY / 2) < MAPY) && ((y + cursorY - SCRY / 2) > -1))
          //renderchar(x + cursorX - SCRX/2,y + cursorY - SCRY/2,x+1,y+1);
          renderchar(x + cursorX - SCRX / 2, y + cursorY - SCRY / 2, x, y);
        else
          F_MVputch(x + 1, y + 1, ' ');
      else
        F_MVputch(x + 1, y + 1, ' ');
}