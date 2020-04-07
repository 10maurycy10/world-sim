#include "toolkit.h"
#include <SDL.h>

#define C_CHAR_X_NUM 16
#define C_CHAR_Y_NUM 16
// RRBBGG

#define encodeC(FR, FG, FB, BR, BB, BG) (FR << 7 | FG << 5 | FB << 3 | BR | BG << 1 | BB << 2) //F* are 0-3 , B* are 0-1

#define extractFR(a) ((a & 0b110000000) >> 7) * 85
#define extractFG(a) ((a & 0b001100000) >> 5) * 85
#define extractFB(a) ((a & 0b000011000) >> 3) * 85

#define extractBR(a) ((a & 0b000000001) >> 0) * 255
#define extractBG(a) ((a & 0b000000010) >> 1) * 255
#define extractBB(a) ((a & 0b000000100) >> 2) * 255

enum FRONT_COLORS { FRONT_COLORS_RED = encodeC(3, 0, 0, 0, 0, 0),
                    FRONT_COLORS_GREEN = encodeC(0, 3, 0, 0, 0, 0),
                    FRONT_COLORS_BLUE = encodeC(0, 0, 3, 0, 0, 0),
                    FRONT_COLORS_TEXT = encodeC(3, 3, 3, 0, 0, 0) };

enum BACK_COLORS { BACK_COLORS_RED = encodeC(0, 0, 0, 1, 0, 0),
                   BACK_COLORS_GREEN = encodeC(0, 0, 0, 0, 1, 0),
                   BACK_COLORS_BLUE = encodeC(0, 0, 0, 0, 0, 1),
                   BACK_COLORS_WHIGHT = encodeC(0, 0, 0, 1, 1, 1) };

SDL_Window *gWindow;
SDL_Surface *gWindowSurface;
SDL_Surface *gFont;
int newlinex = 0;
struct IntVect2 C_cursor = {0,0};
struct IntVect2 fontSize;
struct IntVect2 windowSize;

void C_mvaddch(int x, int y, int ch, int c) {
  int collom = ch / C_CHAR_X_NUM;
  int row = ch % C_CHAR_X_NUM;

  int fontX = row * fontSize.x;
  int fontY = collom * fontSize.y;

  int scrX = x * fontSize.x;
  int scrY = y * fontSize.y;

  SDL_Rect dst = {scrX, scrY, fontSize.x, fontSize.y};
  SDL_Rect scr = {fontX, fontY, fontSize.x, fontSize.y};

  SDL_FillRect(gWindowSurface, &dst, SDL_MapRGB(gWindowSurface->format, extractBR(c), extractBG(c), extractBB(c)));
  SDL_SetSurfaceColorMod(gFont, extractFR(c), extractFG(c), extractFB(c));
  SDL_BlitSurface(gFont, &scr, gWindowSurface, &dst);
}

void C_addch(char ch, int attr) { //auto manage cursor
    if (ch == '\n') {
        C_cursor.y++;
        C_cursor.x = newlinex;
    } else {
        C_mvaddch(C_cursor.x, C_cursor.y, ch, attr);
        C_cursor.x++;
    }
}

void C_puts(char* str, int attr) {
    int i = 0;
    while (str[i]) {
        C_addch(str[i], attr);
        i++;
    }
}

void C_init() {
  gWindow = SDL_CreateWindow("World of Doom", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
  gWindowSurface = SDL_GetWindowSurface(gWindow);
}

void C_loadFont(char *font) {
  gFont = SDL_LoadBMP(font);
  fontSize.x = gFont->w / C_CHAR_X_NUM;
  fontSize.y = gFont->h / C_CHAR_Y_NUM;

  windowSize.x = gWindowSurface->w / fontSize.x;
  windowSize.y = gWindowSurface->h / fontSize.y;

  SDL_SetColorKey(gFont, 1, SDL_MapRGB(gFont->format, 255, 0, 0));
}

void pollIo(void (*callback)(int)) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      AUTO_CASE(SDL_KEYDOWN, C_puts(SDL_GetKeyName(event.key.keysym.sym),FRONT_COLORS_TEXT); C_puts("\n",FRONT_COLORS_TEXT); callback(event.key.keysym.sym))

      /* SDL_QUIT event (window close) */
      AUTO_CASE(SDL_QUIT,
                SDL_Quit();
                _Exit(0);)

      AUTO_CASE(SDL_WINDOWEVENT_RESIZED, gWindowSurface = SDL_GetWindowSurface(gWindow); windowSize.x = gWindowSurface->w / fontSize.x; windowSize.y = gWindowSurface->h / fontSize.y;)
    }
  }
}

void C_refresh() {
  //SDL_BlitScaled(gFont,NULL,gWindowSurface,NULL);
  SDL_UpdateWindowSurface(gWindow);
}