#include "mats.c"

struct Tyle {
  int32_t temperature; // 1000 = 0c  1100 = 100c
  uint16_t Lmat;
  uint16_t Fmat;
  bool discoverd;
  struct LData {
    int16_t mosstimer;
  } LairData;
};

void dotyle(int x, int y, struct Tyle* dst, struct Tyle** map) {
  *dst = map[x][y];
}

void drawTyle(int sx,int sy,struct Tyle tyle) {
  C_mvaddch(sx, sy, gMats[tyle.Lmat].matTexture[0], gMats[tyle.Lmat].matCol[0]);
}