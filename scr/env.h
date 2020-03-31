double gSealeval;
int gGrasregrow;
int gMSPT;
int gRavaPlaceTemp;
int gMapx;
int gMapy;
int gWindowx;
int gWindowy;

char *gRFile; //raw file over ride

const char *gVerson = "0.12"; //game verson

#define MAPY gMapy //map size
#define MAPX gMapx
#define SCRY ((gWindowy) - 2) //the size of the map vew port
#define SCRX ((gWindowx - 50))
#define HELPY (gWindowy - 1)
#define HELPX ((48))
#define HELPXSTART (SCRX + 1)


struct Raw {
  int stoneMelt;    //normlized temp
  int grassRegrow; //time
  double seaLeval;     //normlized elivatin
};



void loadObj(struct Raw *data) {
  gSealeval = 0;
  gGrasregrow = 60;
  gMSPT = 34;
}

int frame = 0;