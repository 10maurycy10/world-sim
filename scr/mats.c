#define MATS 1042
#include "pharse.c"
struct MAT {
  char Texture[3];
  bool mossy;
  char *desc;
  char *name;
  int nameCol;
  int Melt;
  int DecompTemp;
  int DecompTo;
  int Col[3];
  bool Void;
  bool grownd;
};

int gNumMats = 0;
struct MAT gMats[MATS];

int MAT_STONE, MAT_GRASS, MAT_AIR;

int getMatFromName(char *name) {
  int i = 0;
  while (i < gNumMats) {
    //printf("getMatFromName.testing %d %p %p\n",i,name,gMats[i].name);
    if (!SDL_strcmp(name, gMats[i].name)) {
      //printf("getMatFromName.sucses\n");
      return i;
    }
    i++;
  }
  return -1;
}

void loadMatRaw() {

  gNumMats = 0;

  gMats[MAT_GRASS].Texture[0] = ',';
  gMats[MAT_STONE].Texture[0] = '#';
  gMats[MAT_GRASS].Col[0] = encodeC(0, 3, 0, 0, 0, 0);
  gMats[MAT_STONE].Col[0] = encodeC(3, 3, 3, 0, 0, 0);

  void req(char *tag, int *i, struct Str *data) {
    if (!TockenMatch(tag, i, data)) {
      char *got = getID(i, data);
      //printf("reqired in mat raws \"%s\" \"%s\"\n",tag,got);
      free(got);
    }
  };

  int id = 0;
  void pharseMat(struct Str * data, int *i) { //crashes when redifining
    skipTextUntilNonWhiteSpace(i, data);
    if (!TockenMatch(":", i, data)) {
      printf("empty mat");
      return;
    }
    char *name = getID(i, data);
    int cid = getMatFromName(name);
    printf("\"%s\"", name);
    if (cid == -1) {
      gNumMats++;
      cid = id;

      gMats[id].name = name;
      gMats[id].mossy = 0;
      gMats[id].Void = 0;
      gMats[id].grownd = 0;
      gMats[id].DecompTo = -1;
      gMats[id].Melt = __INT_MAX__;
      gMats[id].desc = "MISING DESC";
      gMats[id].Col[0] = 0;
      gMats[id].Col[1] = 0;
      gMats[id].Col[2] = 0;
      gMats[id].nameCol = FRONT_COLORS_BLUE;
      id++;
    }

    skipTextUntilNonWhiteSpace(i, data);
    //printf(": %d %c\n", cid, data->data[*i]);

    if (TockenMatch(":", i, data)) {
      void Tvoid(struct Str * data, int *i) {
        gMats[cid].Void = 1;
      }
      void Tdesc(struct Str * data, int *i) {
        req(":",i,data);
        gMats[cid].desc = getstrtag(i,data);
        int d[3];
        for (int e = 0; e < 3; e++) {
          req(":", i, data);
          skipTextUntilNonWhiteSpace(i, data);
          char *text = getID(i, data);
          sscanf(text, "%d", &d[e]);
          free(text);
          skipTextUntilNonWhiteSpace(i, data);
        }
        gMats[cid].nameCol = encodeC(d[0],d[1],d[2],0,0,0);
      }
      void Tcolor(struct Str * data, int *i) {
        int d[9];
        for (int e = 0; e < 9; e++) {
          req(":", i, data);
          skipTextUntilNonWhiteSpace(i, data);
          char *text = getID(i, data);
          sscanf(text, "%d", &d[e]);
          free(text);
          skipTextUntilNonWhiteSpace(i, data);
        }
        for (int e = 0; e < 3; e++) {
          gMats[cid].Col[e] |= encodeC(d[e * 3 + 0], d[e * 3 + 1], d[e * 3 + 2], 0, 0, 0);
        }
      }
      void Tmelt(struct Str * data, int *i) {
        if (!TockenMatch(":", i, data)) {
          gMats[id].Melt = __INT_MAX__;
        }
        int melt;
        char *text = getID(i, data);
        sscanf(text, "%d", &melt);
        gMats[cid].Melt = melt;
        free(text);
      }
      void Tdraw(struct Str * data, int *i) {
        //printf("loadMatRaw.pharseMat.Tdraw");
        skipTextUntilNonWhiteSpace(i, data);
        req(":", i, data);
        skipTextUntilNonWhiteSpace(i, data);
        req("\"", i, data);
        gMats[cid].Texture[0] = data->data[*i + 0];
        gMats[cid].Texture[1] = data->data[*i + 1];
        gMats[cid].Texture[2] = data->data[*i + 2];
        (*i) += 3;
        req("\"", i, data);
      }

      while (!TockenMatch("]", i, data)) {
        skipTextUntilTag(i, data);
        if (TockenMatch("[", i, data)) {
          skipTextUntilNonWhiteSpace(i, data);

          checkTag("DRAW", i, data, Tdraw);
          checkTag("COLOR", i, data, Tcolor);
          checkTag("VOID", i, data, Tvoid);
          checkTag("MELT", i, data, Tmelt);
          checkTag("DESC", i, data, Tdesc);

          //printf("loadMatRaw.pharseMat: done checking at %d %c\n",*i,data->data[*i]);

          skipTextUntilNonWhiteSpace(i, data);
          if (!TockenMatch("]", i, data)) {
            //printf("error in mats TAG_UNCOSED '%c'\n", data->data[*i]);
            return;
          }
          //printf("loadMatRaw.pharseMat: mchd tag close %d %c\n",*i,data->data[*i]);
        }
        skipTextUntilNonWhiteSpace(i, data);
      }
      //Sprintf("loadMatRaw.pharseMat: leaving close %d %c\n",*i,data->data[*i]);
      (*i)--;
    } else {
      printf("no mat body!!\n");
    }
  };

  void pharser(struct Str * data) {
    gNumMats = 0;
    int i = 0;
    while (i < data->size) {
      skipTextUntilTag(&i, data);
      if (TockenMatch("[", &i, data)) {
        skipTextUntilNonWhiteSpace(&i, data);

        checkTag("MAT", &i, data, pharseMat);

        skipTextUntilNonWhiteSpace(&i, data);
        if (!TockenMatch("]", &i, data)) {
          printf("error in raws TAG_UNCOSED\n");
          return;
        }
      } else if (i < data->size) { //if not got tag start but not EOF
        printf("error in raws NO VALID TAGS at %d\n", i);
        return;
      }
    }
  };

  readFile("data/raw/raw.txt", &pharser);

  MAT_GRASS = getMatFromName("GRASS");
  MAT_STONE = getMatFromName("STONE");
  MAT_AIR = getMatFromName("AIR");
}
