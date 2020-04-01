#include <stdint.h>
#include <SDL.h>

struct Save {
  char magic[5];
  int16_t X;
  int16_t Y;
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

  printf("%p",data.savefile);

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
