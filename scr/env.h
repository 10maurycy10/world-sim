double gSealeval;
double gLavatemp;
double gLavaPlaceTemp;
int gGrasregrow;
int gMSPT;
int gRavaPlaceTemp;

char* gRFile; //raw file over ride

const char* gVerson = "0.9"; //game verson

#define MAPY 40 //map size
#define MAPX 80
#define SCRY 20 //ccr size
#define SCRX 40

struct Raw {
    double stoneMelt; //normlized temp
    int grassRegrow; //time
    double seaLeval; //normlized elivatin
    int MSPT; //mili secons per tick
    int lava;
};

struct tyle **map;
struct tyle **nmap;

void loadObj(struct Raw* data) {
  gSealeval = data -> seaLeval;
  gLavatemp = data -> stoneMelt;
  gGrasregrow = data -> grassRegrow;
  gLavaPlaceTemp = data -> lava;
  gMSPT = data -> MSPT;
}

int frame = 0;