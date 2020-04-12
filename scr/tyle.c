#include "mats.c"

struct Tyle {
  int32_t temperature; // 1000 = 0c  1100 = 100c
  uint16_t Lmat;
  uint16_t Fmat;
  bool discoverd;
  struct LData {
    int16_t mosstimer;
  } LairData;
} __attribute__((packed));

void dotyle(int x, int y, struct Tyle* dst, struct Tyle** map) {
  *dst = map[x][y];
}

void drawTyle(int sx,int sy,struct Tyle tyle) {
  int ch = gMats[tyle.Lmat].Texture[0];
  int attr = gMats[tyle.Lmat].Col[0];
  if (tyle.temperature > gMats[tyle.Lmat].Melt) {
    ch = gMats[tyle.Lmat].Texture[1];
    attr = gMats[tyle.Lmat].Col[1];
  }
  C_mvaddch(sx, sy, ch, attr);
}