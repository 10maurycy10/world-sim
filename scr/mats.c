#define MATS 1042
#include "pharse.c"
struct MAT {
  char Texture[3];
  bool mossy;
  char *desc;
  char *name;
  int Melt;
  int DecompTemp;
  int DecompTo;
  int Col[3];
  bool Void;
  bool grownd;
};

int gNumMats = 0;
struct MAT gMats[MATS];

enum {MAT_GRASS = 0, MAT_STONE = 1};

int getMatFromName(char* name) {
  int i = 0;
  while (i > gNumMats) {
    if (!SDL_strcmp(name,gMats[i].name)) {
      return i;
    }
    i++;
  }
  return -1;
}

void loadMatRaw() {
    gMats[MAT_GRASS].Texture[0] = ',';
    gMats[MAT_STONE].Texture[0] = '#';
    gMats[MAT_GRASS].Col[0] = encodeC(0,3,0,0,0,0);
    gMats[MAT_STONE].Col[0] = encodeC(3,3,3,0,0,0);

    int id = 0;
    void pharseMat(struct Str* data, int* i) { //crashes when redifining
        int cid;
        TockenMatch(":",i,data);

        char* name = getID(i,data);
        if((cid = getMatFromName(name)) == -1) {
          gNumMats ++;
          cid = id;
          id++;

          gMats[id].name = name;
          gMats[id].mossy = 0;
          gMats[id].Void = 0;
          gMats[id].grownd = 0;
          gMats[id].DecompTo = -1;
          gMats[id].desc = 0;
        }

        printf("%d ",cid);

        TockenMatch(":",i,data);

        void Tvoid(struct Str* data, int* i) {}
        void Tdraw(struct Str* data, int* i) {
          TockenMatch(":",i,data);
          gMats[cid].Texture[0] = data->data[*i + 0];
          gMats[cid].Texture[1] = data->data[*i + 1];
          gMats[cid].Texture[2] = data->data[*i + 2];
          (*i) += 3;
        }

        while (!TockenMatch("]",i,data)) {
          skipTextUntilTag(i, data);
          if (TockenMatch("[",i,data)) {
            skipTextUntilNonWhiteSpace(i,data);

            checkTag("DRAW",i,data,Tdraw);

            skipTextUntilNonWhiteSpace(i,data);
            if (!TockenMatch("]",i,data)) {
              printf("error in mats TAG_UNCOSED '%c'\n",data->data[*i]);
              return;
            }
          }
        }
        (*i) --;
    };

    void pharser(struct Str* data) {
      gNumMats = 0;
      int i = 0;
      while (i < data->size) {
        skipTextUntilTag(&i, data);
        if (TockenMatch("[",&i,data)) {
          skipTextUntilNonWhiteSpace(&i,data);

          checkTag("MAT",&i,data,pharseMat);

          skipTextUntilNonWhiteSpace(&i,data);
          if (!TockenMatch("]",&i,data)) {
            printf("error in raws TAG_UNCOSED\n");
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
