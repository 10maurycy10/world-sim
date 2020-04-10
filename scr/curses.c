#include "toolkit.h"
#include <SDL.h>

#define C_CHAR_X_NUM 16
#define C_CHAR_Y_NUM 16
// RRBBGG

#define f_block 0xDB

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
                    FRONT_COLORS_TEXT = encodeC(3, 3, 3, 0, 0, 0),
                    FRONT_COLORS_DIM = encodeC(2, 2, 2, 0, 0, 0),
                    FRONT_COLORS_DARK_RED = encodeC(1,0,0,0,0,0) };

enum BACK_COLORS { BACK_COLORS_RED = encodeC(0, 0, 0, 1, 0, 0),
                   BACK_COLORS_GREEN = encodeC(0, 0, 0, 0, 1, 0),
                   BACK_COLORS_BLUE = encodeC(0, 0, 0, 0, 0, 1),
                   BACK_COLORS_WHIGHT = encodeC(0, 0, 0, 1, 1, 1) };

struct Viewport {
  struct IntVect2 tl;
  struct IntVect2 br;
};

SDL_Window *gWindow;
SDL_Surface *gWindowSurface;
SDL_Surface *gFont;
int newlinex = 0;
struct IntVect2 C_cursor = {0, 0};
struct IntVect2 fontSize;
struct IntVect2 windowSize;
struct Viewport screen = {{0, 0}, {0, 0}};

void C_clear() {
  SDL_FillRect(gWindowSurface, NULL, SDL_MapRGB(gWindowSurface->format, 0, 0, 0));
}

void move(int x, int y) {
  C_cursor.x = x;
  C_cursor.y = y;
}

struct Viewport C_mkPort(int x, int y, int x2, int y2) {
  struct Viewport self;
  self.tl.x = x;
  self.tl.y = y;
  self.br.x = x2;
  self.br.y = y2;
  return self;
}

void C_focus(struct Viewport target) {
  newlinex = target.tl.x + 1;
  move(target.tl.x + 1, target.tl.y + 1);
}

void C_focus_title(struct Viewport target) {
  newlinex = target.tl.x + 3;
  move(target.tl.x + 3, target.tl.y);
}

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

void C_vline(int x, int s, int e, int ch, int attr) {
  for (int y = s; !(y > e); y++) {
    C_mvaddch(x, y, ch, attr);
  }
}

void C_hline(int y, int s, int e, int ch, int attr) {
  for (int x = s; !(x > e); x++) {
    C_mvaddch(x, y, ch, attr);
  }
}

void C_box(struct Viewport target, int attr) {
  C_hline(target.br.y, target.tl.x, target.br.x, f_block, attr);
  C_hline(target.tl.y, target.tl.x, target.br.x, f_block, attr);
  C_vline(target.br.x, target.tl.y, target.br.y, f_block, attr);
  C_vline(target.tl.x, target.tl.y, target.br.y, f_block, attr);
}

void C_addch(char ch, int attr) { //auto manage cursor
  if (ch == '\n') {
    C_cursor.y++;
    C_cursor.x = newlinex;
  } if (ch == '\t') {} else {
    C_mvaddch(C_cursor.x, C_cursor.y, ch, attr);
    C_cursor.x++;
  }
}

void C_puts(const char *str, int attr) {
  int i = 0;
  while (str[i]) {
    C_addch(str[i], attr);
    i++;
  }
}

void C_printf(const char *formatString, int attrs, int params, ...) {
  va_list extra;
  va_start(extra, params);
  for (int i = 0; formatString[i]; i++) {
    if (formatString[i] == '%') {
      i++;
      switch (formatString[i]) {
        AUTO_CASE('s',
                  C_puts(va_arg(extra, char *), attrs))
        AUTO_CASE('d',
                  char buffer[sizeof(char) * sizeof(int) * 4 + 1];
                  sprintf(buffer, "%d", va_arg(extra, int));
                  C_puts(buffer, attrs);)
        AUTO_CASE('x',
                  char buffer[sizeof(char) * sizeof(int) * 4 + 1];
                  sprintf(buffer, "%x", va_arg(extra, int));
                  C_puts(buffer, attrs);)
        AUTO_CASE('p',
                  char buffer[sizeof(char) * sizeof(void *) * 4 + 1];
                  sprintf(buffer, "%p", va_arg(extra, void *));
                  C_puts(buffer, attrs);)
      }
    } else {
      C_addch(formatString[i], attrs);
    }
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

  screen.tl.x = 0;
  screen.tl.y = 0;
  screen.br.y = windowSize.y - 1;
  screen.br.x = windowSize.x - 1;

  SDL_SetColorKey(gFont, 1, SDL_MapRGB(gFont->format, 255, 0, 0));
}

void pollIo(void (*callback)(int,int)) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      AUTO_CASE(SDL_KEYDOWN, callback(event.key.keysym.sym,event.key.keysym.mod))

      /* SDL_QUIT event (window close) */
      AUTO_CASE(SDL_QUIT,
                SDL_Quit();
                _Exit(0);)

      AUTO_CASE(
          SDL_WINDOWEVENT,
          if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
            gWindowSurface = SDL_GetWindowSurface(gWindow);
            windowSize.x = gWindowSurface->w / fontSize.x;
            windowSize.y = gWindowSurface->h / fontSize.y;
            screen.tl.x = 0;
            screen.tl.y = 0;
            screen.br.y = windowSize.y - 1;
            screen.br.x = windowSize.x - 1;
          })
    }
  }
  printf("%s", SDL_GetError());
}

void C_refresh() {
  //SDL_BlitScaled(gFont,NULL,gWindowSurface,NULL);
  SDL_UpdateWindowSurface(gWindow);
}