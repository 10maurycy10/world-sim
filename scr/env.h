double gSealeval;
double gLavatemp;
double gLavaPlaceTemp;
int64_t gGrasregrow;
int64_t gMSPT;
int64_t gRavaPlaceTemp;
int64_t gMapx;
int64_t gMapy;
int64_t gWindowx;
int64_t gWindowy;


char* gRFile; //raw file over ride
char* gSfile;

const char* gVerson = "0.12"; //game verson

#define MAPY gMapy //map size
#define MAPX gMapx
//#define TEST
#define SCRY (gWindowy-4) //sr size
#define SCRX (gWindowx-4)

struct Raw {
    double stoneMelt; //normlized temp
    int64_t grassRegrow; //time
    double seaLeval; //normlized elivatin
    int64_t MSPT; //milli secons per tick
    int64_t lava;
};

struct Save {
  int64_t X;
  int64_t Y;
  struct Tyle data;
};

struct Tyle **map;
struct Tyle **nmap;

void loadObj(struct Raw* data) {
  gSealeval = data -> seaLeval;
  gLavatemp = .5;
  gGrasregrow = data -> grassRegrow;
  gLavaPlaceTemp = data -> lava;
  gMSPT = data -> MSPT;
}

int64_t frame = 0;