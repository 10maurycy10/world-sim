
bool loadWorld();

struct WorldTyle {
  int avgTmp;
  int avgElivation;
  int waterLeval;
};

int gWorldX;
int gWorldY;
struct WorldTyle **world;

void drawWorldTtyle(int Wx, int Wy, int x, int y) {
  if (world[Wx][Wy].avgTmp > 3000) {
    F_ATTR(F_COLOR_PAIR(C_MAGMA));
    F_MVputch(x, y, '~');
  } else {
    F_ATTR(F_COLOR_PAIR(C_GRASS));
    F_MVputch(x, y, '.');
  }
  if (Wx == cursorX && Wy == cursorY) {
    F_ATTR(F_COLOR_PAIR(C_HIGH));
    F_MVputch(x, y, 'X');
  }
}

void drawWorld() {
  for (int x = 0; x < gWorldX; x++)
    for (int y = 0; y < gWorldX; y++) {
      drawWorldTtyle(x, y, x, y);
    }
}

void genWorld() {
  srand(clock());

  gWorldX = 16;
  gWorldY = 16;

  if (world) {
    for (int i = 0; i < gWorldX; i++) {
      free(world[i]);
    }
    free(world);
  }

  world = malloc(gWorldX * (sizeof(void *)));
  for (int i = 0; i < gWorldX; i++) {
    world[i] = malloc((sizeof(struct WorldTyle)) * gWorldY);
  }
}

void getWorld() {
  if (loadWorld()) {
    genWorld();
  }
}

