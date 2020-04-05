#include <stdint.h>
#include <SDL.h>

struct Save {
  char magic[5];
  int16_t Xsize;
  int16_t Ysize;
  int16_t Xcursor;
  int16_t Ycorsor;
  struct Tyle data;
};

#define SAVE_MAGIC ((char *)("ARMS")) //MUST BE LESS THEN 5 CHARS`

void loadSave(struct Config data) {

  struct Save *s = malloc((data.savefile->size(data.savefile)) + 1);
  SDL_RWseek(data.savefile, 0, RW_SEEK_SET);
  data.savefile->read(data.savefile, s, 1, data.savefile->size(data.savefile)); //copy into mem

  //_Exit(0);


  if (!(s->magic[0] == SAVE_MAGIC[0] && s->magic[1] == SAVE_MAGIC[1] && s->magic[2] == SAVE_MAGIC[2] && s->magic[3] == SAVE_MAGIC[3])) {
    printf("this is NOT a save!!");
    return;
  }

  if ((data.savefile)->size(data.savefile) != (sizeof(struct Save) - sizeof(struct Tyle) + (sizeof(struct Tyle) * (s)->Ysize * (s)->Xsize) )) {
    printf("save is to small");
    return;
  }


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

  cursorY = s->Ycorsor;
  cursorX = s->Xcursor;
  gMapx = (s)->Xsize;
  gMapy = (s)->Ysize;

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
  save->Ysize = MAPY;
  save->Xsize = MAPX;
  save->Ycorsor = cursorY;
  save->Xcursor = cursorX;

  for (int x = 0; x < MAPX; x++)
    for (int y = 0; y < MAPY; y++)
      OFFSET(save->data, x + y * MAPX) = map[x][y];

  SDL_RWseek(data.savefile, 0, RW_SEEK_SET);
  if (data.savefile->write(data.savefile, save, 1, sizeof(struct Save) - sizeof(struct Tyle) + sizeof(struct Tyle) * numTyle)  != (sizeof(struct Save) - sizeof(struct Tyle) + sizeof(struct Tyle) * numTyle)) {
    F_ATTR(F_COLOR_PAIR(C_TEXT));
    F_mbox("failed to save world");
  }

  free(save);
}


bool loadWorld() { //retun 1 if failed
  return 1;
}