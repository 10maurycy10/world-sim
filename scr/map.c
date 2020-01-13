#ifndef false
    #define false 0
    #define true 1
#endif


struct tyle {
  double elivation; //0 = deep ossen 1 = mounten .5 = sea leval
  double temperature; //0 = cold  1 = hot
  int type;
};

enum  S {T_STONE,T_GRASS};

void genaratemap() { //reset the map
 for (int y = 0;y<MAPY;y++) {
   for (int x = 0;x<MAPX;x++) {
     map[x][y].elivation = 0.5f;
     map[x][y].temperature = 0.0f;
     map[x][y].type = T_GRASS;
   }
 }
}

WINDOW* mapw;

enum colors {C_TEXT,C_HIGH,C_GRASS,C_STONE,C_MAGMA,C_WATER};
enum icons {CH_STONE =  '#',CH_WATER = '~'};
char* gGrass = ",'.`"; 
#define gGrasscount 4

void renderchar(int x,int y,int xpos,int ypos) {//render a x:x(map) y:y(map) pos: pos on screan
    int attr = 0;
    int tylech = 0;
    switch (map[x][y].type){
    case T_GRASS:
        attr |= COLOR_PAIR(C_GRASS);
        tylech |= gGrass[( (y^(gGrasscount / 2)) ^ ((x+1)^((gGrasscount))))%gGrasscount];
        break;
    case T_STONE:
        attr |=  COLOR_PAIR(C_STONE);
        tylech |= CH_STONE;

    break;
    default:
        tylech  = ' ';
        break;
    }
    if ((y == cursorY)&&(x == cursorX)&&((frame%60)>30))  {
      tylech = 'X';
      attr =  (COLOR_PAIR(C_HIGH));
    } else {

    }
    
    mvwaddch(mapw,ypos,xpos,tylech | attr);
}

void maprender() {
  for (int x = 0;x<SCRX;x++)
    for (int y = 0;y<SCRY;y++)
      if (((x + cursorX - SCRX/2)<MAPX) && ((x + cursorX - SCRX/2) > -1))
        if (((y + cursorY - SCRY/2) < MAPY) && ((y + cursorY - SCRY / 2)>-1))
          renderchar(x + cursorX - SCRX/2,y + cursorY - SCRY/2,x,y+1);
        else
          mvwaddch(mapw,y+1,x,' ');
      else
        mvwaddch(mapw,y+1,x,' ');

  box(mapw, '|' , '-' );
  wrefresh(mapw);
}
