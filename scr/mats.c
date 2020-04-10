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
