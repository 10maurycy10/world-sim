#include "profiler.c"
#include "toolkit.h"
#include <SDL.h>
#include <SDL_main.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

enum RS { RS_CLOSE,
          RS_GAME,
          RS_MAIN };
enum colors {
  C_TEXT,
  C_OK,
  C_YELOW,
  C_ERROR,
  C_FAIL,
  C_HIGH,
  C_GRASS,
  C_STONE,
  C_MAGMA,
  C_WATER,
  C_DIM
};

enum MEN { MEN_MAIN,
           MEN_INSPECT,
           MEN_PLACE };
int menueState = MEN_MAIN;

void gend();
#include "env.h"
#include "font.c"
int cursorX = 1, cursorY = 1;
#include "config.c"
#include "map.c"
#include "raw.c"

enum controls {
  K_OK = SDLK_RETURN,
  K_LEFT = SDLK_LEFT,
  K_UP = SDLK_UP,
  K_DOWN = SDLK_DOWN,
  K_RIGHT = SDLK_RIGHT,
  K_INSPECT = SDLK_k,

  K_EXIT = SDLK_q,
  K_RESTART = SDLK_r,

  K_LAND = SDLK_z,
  K_WATER = SDLK_x,
  K_LAVA = SDLK_c,

  K_load = SDLK_o,
  K_save = SDLK_p,
  K_debug = SDLK_ESCAPE
};

int running;

bool gameIo(int key, struct Config data) {

   printf("%x",key);
  
  switch (menueState) {
    case MEN_MAIN:
      switch (key) {
        case K_EXIT:
          running = RS_MAIN;
          for (int i = 0;i < 256;i++)
            free(matNames[i]);
          break;
        case K_LEFT:
          if (cursorX > 4)
            cursorX -= 5;
          break;
        case K_RIGHT:
          if (cursorX < MAPX - 5)
            cursorX += 5;
          break;
        case K_UP:
          if (cursorY > 4)
            cursorY -= 5;
          break;
        case K_DOWN:
          if (cursorY < MAPY - 5)
            cursorY += 5;
          break;
        case K_INSPECT:
          menueState = MEN_INSPECT;
          break;
      }
      break;
    case MEN_INSPECT:
      switch (key) {
        case K_EXIT:
          menueState = MEN_MAIN;
          break;
        case K_LEFT:
          if (cursorX > 0)
            cursorX -= 1;
          break;
        case K_RIGHT:
          if (cursorX < MAPX - 1)
            cursorX += 1;
          break;
        case K_UP:
          if (cursorY > 0)
            cursorY -= 1;
          break;
        case K_DOWN:
          if (cursorY < MAPY - 1)
            cursorY += 1;
          break;
      }
      break;
      default:
        menueState = MEN_MAIN;
  }
  switch (key) {
    case 0:
      return 1;

    case K_LAND:
      map[cursorX][cursorY].Lmat = MAT_GRASS;
      map[cursorX][cursorY].temperature = 0.0f;
      break;
    case K_WATER:
      map[cursorX][cursorY].Lmat = MAT_STONE;
      map[cursorX][cursorY].temperature = 0.0f;
      map[cursorX][cursorY].LairData.mosstimer = -1;
      break;
    case K_LAVA:
      map[cursorX][cursorY].temperature = 10000;
      break;

    case K_debug:
      F_move(0, 20);
      printf("Ltype: %d, ", map[cursorX][cursorY].Lmat);
      printf("Ftype: %d, ", map[cursorX][cursorY].Fmat);
      printf("has space %s", gMats[map[cursorX][cursorY].Lmat].matVoid?"true":"false");
      F_more();
  }

  return 1;
}

struct Config dataconfig;
void gameloop() {
  pInit();
  struct Raw rawdata;
  F_ATTR(F_COLOR_PAIR(C_TEXT));
  F_clear();
  F_move(0, 0);
  F_ATTR(F_COLOR_PAIR(C_TEXT));
  F_printw("[*] loading objects...", 0, 0);
  readraw(dataconfig.rawfile, &rawdata);
  loadObj(&rawdata);
  SDL_Event e;
  F_ATTR(F_COLOR_PAIR(C_OK));
  F_printw("\t\t[DONE]\n", 0, 0);

  F_ATTR(F_COLOR_PAIR(C_TEXT));
  F_printw("[*] making map", 0, 0);

  int *seedblock = malloc(sizeof(int) * 100);
  int seed = 0;
  for (int i = 0; i < 100; i++)
    seed += seedblock[i];
  free(seedblock);
  genaratemap(seed);

  F_ATTR(F_COLOR_PAIR(C_OK));
  F_printw("\t\t[DONE]\n", 0, 0);
  F_ATTR(F_COLOR_PAIR(C_TEXT));
  //F_more();
  F_getmaxxy(gWindowx, gWindowy);
  // printf("X: %d Y: %d",(int)gWindowx,(int)gWindowy);

  int lastFrame = 0;
  int lastTick = 0;
  int mspt = 0;
  int key = 0;
  profile(T_spin);

  while (running == RS_GAME) {
    if (((CLOCKS_PER_SEC / 1000) * gMSPT) <= (clock() - lastTick)) {
      mspt = (clock() - lastTick);
      lastTick = clock();
      profile(T_tick);
      ticktyles();
      frame++;
      profile(T_draw);
      F_clear();
      F_ATTR(C_DIM);
      F_box(0, 0, SCRX, SCRY + 1, true);
      F_box(HELPXSTART, 0, HELPX + HELPXSTART, HELPY, true);
      F_ATTR(F_COLOR_PAIR(C_TEXT));
      F_move(HELPXSTART + 2, 2);
      switch (menueState) {
        case MEN_MAIN:
          F_printw("Esc : Debug\n", HELPXSTART + 2, 0);
          F_printw("K : Inspect\n", HELPXSTART + 2, 0);
          break;
        case MEN_INSPECT:
          if (!map[cursorX][cursorY].discoverd)
            F_printw(" un-known\n", 0, HELPXSTART + 2);
          else {
            F_printw("%s",HELPXSTART + 2,1,(!gMats[map[cursorX][cursorY].Lmat].matVoid)?matNames[map[cursorX][cursorY].Fmat]:matNames[map[cursorX][cursorY].Fmat]);
            if (gMats[map[cursorX][cursorY].Lmat].matVoid)
              F_printw(" floor",HELPXSTART + 2,0);
          }
      }
      F_move(0, 0);
      if (mspt != 0)
        F_printw("\ttps: %d", 0, 1, (int)(1000 / (mspt)) + 1);
      else
        F_printw("\ttps: infinity", 0, 0);
      maprender();
      
      profile(T_present);
      F_refresh();
      profile(T_spin);
    }
    //lastFrame = clock();

    profile(T_input);
    key = 0;
    while (F_gete(&e)) {
      //printf("event");
      if (e.type == SDL_KEYDOWN) {
        key = e.key.keysym.sym;
        gameIo(key, dataconfig);
      } else if (e.type == SDL_WINDOWEVENT_CLOSE) {
        for (int i = 0;i < 256;i++)
          free(matNames[i]);
        running = RS_CLOSE;
        return;
      }
    }
    // F_MVputch(0,0,'a');
    // F_MVputch(1,1,'b');
    //F_clear();
  }
}

void mainloop() {
  //F_clear();
  SDL_Event e;
  int pos = 0;

  while (running == RS_MAIN) {
    F_clear();

    F_move(0, 0);
    F_ATTR(C_DIM);
    F_box(0, 0, 15, 15, true);
    F_ATTR(C_TEXT);
    F_move(2, 0);
    F_printw("world sim\n\n", 0, 0);
    // F_printInt(pos,16);
    if (pos == 0) {
      F_ATTR(C_TEXT);
      F_printw("\t\tworld sim\n", 0, 0);
    } else {
      F_ATTR(C_DIM);
      F_printw("\t\tworld sim\n", 0, 0);
    }
    if (pos == 1) {
      F_ATTR(C_TEXT);
      F_printw("\t\tabout\n", 0, 0);
    } else {
      F_ATTR(C_DIM);
      F_printw("\t\tabout\n", 0, 0);
    }
    if (pos == 2) {
      F_ATTR(C_TEXT);
      F_printw("\t\texit\n", 0, 0);
    } else {
      F_ATTR(C_DIM);
      F_printw("\t\texit\n", 0, 0);
    }
    F_refresh();
    SDL_Delay(15);

    while (F_gete(&e)) {
      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
          case SDLK_DOWN:
            pos += 1;
            if (pos > 2)
              pos = 2;
            break;
          case SDLK_UP:
            pos -= 1;
            if (pos < 0)
              pos = 0;
            break;
          case K_OK:
            switch (pos) {
              case 0:
                running = RS_GAME;
                break;
              case 2:
                running = RS_CLOSE;
                break;
              case 1:
                F_move(0, 0);
                F_ATTR(C_YELOW);
                F_printw("\n\t         NOPE  \n", 0, 0);
                F_printw("\t               \n", 0, 0);
                F_printw("\tNOPE           \n", 0, 0);
                F_printw("\t     NOPE      \n", 0, 0);
                F_printw("\t          NOPE \n", 0, 0);
                F_printw("\t               \n", 0, 0);
                F_printw("\t               \n", 0, 0);
                F_printw("\t               \n", 0, 0);
                F_printw("\t     NOPE      ", 0, 0);
                F_ATTR(C_DIM);
                F_box(0, 0, F_cursorx, F_cursory + 1, false);
                F_ATTR(C_TEXT);
                F_mbox("world sim\n  by M.Z.  \n");
                break;
            }
            break;
        }
      }
    }
    F_clear();
  }
}

void game(SDL_RWops *configfile) {

#ifdef P
#ifdef TEST
  SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_INFORMATION, "Test build", "This is a test build of the program.\n It may be have a lot of bugs.", NULL);
#endif
#endif

  // printf("F_init.\n");
  F_init();

  // printf("F_initpair.\n");
  F_initpair(C_TEXT, 255, 255, 255, 0, 0, 0);
  F_initpair(C_DIM, 0xA0, 0xA0, 0xA0, 0, 0, 0);
  F_initpair(C_ERROR, 0, 0, 127, 255, 255, 0);

  F_initpair(C_OK, 0, 0, 255, 0, 0, 0);
  F_initpair(C_FAIL, 255, 0, 0, 0, 0, 0);
  F_initpair(C_HIGH, 0xFF, 0x0, 0xFF, 0, 0, 0);
  F_initpair(C_YELOW, 0xff, 0x0, 0xff, 0, 0, 0);

  F_initpair(C_WATER, 0, 255, 0, 0, 0, 0);
  F_initpair(C_MAGMA, 255, 0, 0, 0, 0, 0);
  F_initpair(C_GRASS, 0, 0, 255, 0, 0, 0);
  F_initpair(C_STONE, 255, 255, 255, 0, 0, 0);
  readconfig(configfile, &dataconfig);
  F_load(dataconfig.font);

  running = RS_MAIN;
  menueState = MEN_MAIN;

  while (1) {
    switch (running) {
      case RS_CLOSE:
        F_clear();
        F_refresh();
        gend();
        F_end();
        _Exit(0);
        break;
      case RS_GAME:
        gameloop();
        break;
      case RS_MAIN:
        mainloop();
        break;
    }
  }
  return;
}

void gend() {
  printTag("spin", T_spin);
  printTag("init", T_init);
  printTag("in", T_input);
  printTag("tick", T_tick);

  printTag("draw", T_draw);
  printTag("present", T_present);

  printf("By nVoidPointer (nvoidpointer@gmail.com) (buggybugs@catty)\n");
}
