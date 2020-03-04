double gSealeval;
double gLavatemp;
int64_t gGrasregrow;
int64_t gMSPT;
int64_t gRavaPlaceTemp;
int64_t gMapx;
int64_t gMapy;
int64_t gWindowx;
int64_t gWindowy;

char *gRFile; //raw file over ride

const char *gVerson = "0.12"; //game verson

#define MAPY gMapy //map size
#define MAPX gMapx
//#define TEST
#define SCRY (gWindowy - 2) //the size of the map vew port
#define SCRX (gWindowx - 2)

struct Raw {
  int stoneMelt;    //normlized temp
  int64_t grassRegrow; //time
  double seaLeval;     //normlized elivatin
  int64_t MSPT;        //milli secons per tick
};



void loadObj(struct Raw *data) {
  gSealeval = 0;
  gLavatemp = 2000;
  gGrasregrow = 60;
  gMSPT = 15;
}

int64_t frame = 0;