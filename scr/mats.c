#define MATS 1042
#include "pharse.c"
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

    int id = 0;
    void pharseMat(struct Str* data, int* i) {
        printf("i %d\n",*i);

        id++;
    };

    void pharser(struct Str* data) {
      int i = 0;
      while (i < data->size) {
        printf("i %d %c\n",i,data->data[0]);
        skipTextUntilTag(&i, data);
        printf("i2 %d\n",i);
        if (TockenMatch("[",&i,data)) {
          skipTextUntilNonWhiteSpace(&i,data);

          checkTag("MAT",&i,data,pharseMat);

          skipTextUntilNonWhiteSpace(&i,data);
          if (!TockenMatch("]",&i,data)) {
            printf("error in raws TAG_UNCOSED");
            return;
          }
        } else if (i < data->size) { //if not got tag start but not EOF
          printf("error in raws NO VALID TAGS at %d\n",i);
          return;
        }
      }
    };

    readFile("data/raw/raw.txt",&pharser);
}
