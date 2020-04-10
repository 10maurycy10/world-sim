#define MATS 1042

struct MAT {
  char matTexture[3];
  bool mossy;
  char *mat_name;
  int matMelt;
  int matDecompTemp;
  int matDecompTo;
  int matCol[3];
  bool matVoid;
  bool matgrownd;
};

struct MAT gMats[MATS];

enum {MAT_GRASS = 0, MAT_STONE = 1};

void loadMatRaw() {
    gMats[MAT_GRASS].matTexture[0] = ',';
    gMats[MAT_STONE].matTexture[0] = '#';
    gMats[MAT_GRASS].matCol[0] = encodeC(0,3,0,0,0,0);
    gMats[MAT_STONE].matCol[0] = encodeC(3,3,3,0,0,0);
}
