#include <SDL.h>
#include <math.h>

struct F_Color {
  uint16_t red, blue, green;
};

int F_x_size = 40;
int F_y_size = 80;
int F_cursorx = 1;
int F_cursory = 1;
SDL_Texture *fontT;
SDL_Renderer *gRenderer;

#define h_line 0xc4
#define v_line 0xb3
#define UL_corner 0xDA
#define UR_corner 0xBF
#define LL_corner 0xC0
#define LR_corner 0xD9
#define f_block 0xDB

SDL_Surface *gScreenSurface;
#define F_getmaxxy(x, y)              \
  do {                                \
    x = gScreenSurface->w / F_x_size; \
    y = gScreenSurface->h / F_y_size; \
  } while (0)

#define F_catch(x)                           \
  do {                                       \
    int y;                                   \
    if ((y = (x))) {                         \
      printf("ERROR: %s\n", SDL_GetError()); \
      _Exit(1);                              \
    }                                        \
  } while (0);

#define F_COLOR_PAIR(x) (x)
uint16_t color_pair;
#define F_ATTR(x) (color_pair = x)

SDL_Renderer *gRenderer = NULL;
#define NO_COL 256
struct F_Color F_colors[NO_COL][2];

SDL_Surface *loadSurface(char *path) {
  SDL_Surface *loadedSurface = SDL_LoadBMP(path);
  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
    _Exit(1);
  } else
    return loadedSurface;
}

void F_initpair(int no, int fr, int fb, int fg, int br, int bb, int bg) {
  F_colors[no][0].red = fr;
  F_colors[no][0].green = fg;
  F_colors[no][0].blue = fb;

  F_colors[no][1].red = br;
  F_colors[no][1].green = bg;
  F_colors[no][1].blue = bb;
}

SDL_Window *window = NULL;
SDL_Texture *gBlack;

void F_load(char *font) {
  SDL_Surface *s = loadSurface(font);
  SDL_Surface *black = SDL_CreateRGBSurface(0, 1, 1, 8, 0, 0, 0, 0);

  s = SDL_ConvertSurface(s, gScreenSurface -> format, 0);

  Uint32 colorKey = SDL_MapRGB(s->format, 0xff, 0, 0);
  F_catch(SDL_SetColorKey(s, SDL_TRUE, colorKey));

  int charH = (s->h) / 16;
  int charW = (s->w) / 16;
  F_x_size = charW;
  F_y_size = charH;
  // printf("fsizeX : %d, fsizey : %d",F_x_size , F_y_size);
  //scrbuffer = SDL_CreateRGBSurface(0, gScreenSurface->w, gScreenSurface->h, 8, 0, 0, 0, 0);
  //F_catch(SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF));


  fontT = SDL_CreateTextureFromSurface(gRenderer, s);
  //gBlack = black;
  gBlack = SDL_CreateTextureFromSurface(gRenderer, black);
  SDL_FreeSurface(black);
  SDL_FreeSurface(s);
}

void F_init() {
  F_initpair(0, 255, 255, 255, 0, 0, 0);
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  window = SDL_CreateWindow("[world simulator]", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  SDL_UpdateWindowSurface(window);
  gScreenSurface = SDL_GetWindowSurface(window);
  gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
}

void F_end() {
  SDL_DestroyTexture(fontT);
  //SDL_DestroyRenderer(gRenderer);
  SDL_Quit();
}

void F_refresh() {
  //SDL_BlitScaled(scrbuffer,Null,gScreenSurface,Null);
  //SDL_UpdateWindowSurface(window);
  SDL_RenderPresent(gRenderer);
  //SDL_Delay(0);
  F_catch(SDL_RenderClear(gRenderer));
}

void F_move(int x, int y) {
  F_cursorx = x;
  F_cursory = y;
}

void F_clear() {
  // clear();
  //SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
  //F_catch(SDL_RenderClear(gRenderer));
  F_move(0, 0);
}

bool F_gete(SDL_Event *e) { // polles for a event : true if got a event, false
                            // otherwize
  while (SDL_PollEvent(e)) {
    if (e->type == SDL_QUIT) {
      gend();
      F_end();
      _Exit(0);
    } else if (e->type == SDL_KEYDOWN) {
      return true;
    } else if (e->type == SDL_WINDOWEVENT) {
      switch(e->window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          gScreenSurface -> w = e -> window.data1;
          gScreenSurface -> h = e -> window.data2;
          F_getmaxxy(gWindowx,gWindowy);
          printf("resize\n");
          break;
      }
    }
  }
  return false;
}

void F_more() {
  F_refresh();
  SDL_Event e;
  while (!F_gete(&e))
    ;
}

void F_MVputch(int x, int y, int c) {
  int pixX = F_x_size * x;
  int pixY = F_y_size * y;

  int fontX = (c % 16) * F_x_size;
  int fontY = (c / 16) * F_y_size;

  SDL_Rect scr = {fontX, fontY, F_x_size, F_y_size};
  SDL_Rect dst = {pixX, pixY, F_x_size, F_y_size};
  //SDL_Rect black = {0,0, 1, 1};

  //SDL_BlitScaled(gBlack,&black,scrbuffer,&dst);
  //SDL_BlitScaled(gFont,&scr,scrbuffer,&dst);
  //SDL_texture(gFont,Null,gScreenSurface,Null);

  SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
  SDL_SetTextureColorMod(fontT, F_colors[color_pair][0].red, F_colors[color_pair][0].green, F_colors[color_pair][0].blue);
  F_catch(SDL_RenderCopy(gRenderer, gBlack, NULL, &dst));
  SDL_SetTextureColorMod(gBlack, F_colors[color_pair][1].red, F_colors[color_pair][1].green, F_colors[color_pair][1].blue);
  F_catch(SDL_RenderCopy(gRenderer, fontT, &scr, &dst));
}

void F_putch(int c) {
  F_MVputch(F_cursorx, F_cursory, c);
  F_cursorx++;
}

void F_printInt(int in, int base) {
  int i, rem, len = 0, n;
  n = in;
  while (n != 0) {
    len++;
    n /= base;
  }
  if (len == 0) {
    F_MVputch(F_cursorx + 1, F_cursory, '0');
    F_cursorx++;
  }
  F_cursorx--;
  F_cursorx += len;
  for (i = 0; i < len; i++) {
    rem = in % base;
    in = in / base;
    F_MVputch(F_cursorx - i, F_cursory, "0123456789ABCDEF"[rem]);
  }
  F_cursorx++;
}

void F_printw(char *x,int b, int n, ...) {
  va_list a;
  va_start(a, n);

  for (uint16_t i = 0; x[i];) {
    if (x[i] == '%') {
      i++;
      switch (x[i]) {
        case 's':;
          char *str = va_arg(a, char *);
          for (int e = 0; str[e]; e++) {
            if ((str[e] != '\t') && (str[e] != '\n'))
              F_putch(str[e]);
            else
              F_cursorx++;
            if (str[e - 1] == '\n') {
              F_cursory++;
              F_cursorx = b;
            }
          }
          break;
        case 'd':;
          int ind = va_arg(a, int);
          F_printInt(ind, 10);
          break;
        case 'x':;
          int inx = va_arg(a, int);
          F_printInt(inx, 16);
          break;
        case 'f':;
          int inf = va_arg(a, double);
          F_printInt(floor(inf), 16);
          break;
      }
      i++;
    } else {
      if ((x[i] != '\t') && (x[i] != '\n'))
        F_putch(x[i]);
      else
        F_cursorx++;
      i++;
      if (x[i - 1] == '\n') {
        F_cursory++;
        F_cursorx = b;
      }
    }
  }
}

void F_box(int sx, int sy, int ex, int ey, bool blocky) {
  if (blocky) {
    F_MVputch(sx, sy, f_block);
    F_MVputch(ex, sy, f_block);
    F_MVputch(sx, ey, f_block);
    F_MVputch(ex, ey, f_block);
    for (int i = sx + 1; i < ex; i++)
      F_MVputch(i, sy, f_block);
    for (int i = sx + 1; i < ex; i++)
      F_MVputch(i, ey, f_block);
    for (int i = sy + 1; i < ey; i++)
      F_MVputch(sx, i, f_block);
    for (int i = sy + 1; i < ey; i++)
      F_MVputch(ex, i, f_block);
  } else {
    F_MVputch(sx, sy, UL_corner);
    F_MVputch(ex, sy, UR_corner);
    F_MVputch(sx, ey, LL_corner);
    F_MVputch(ex, ey, LR_corner);
    for (int i = sx + 1; i < ex; i++)
      F_MVputch(i, sy, h_line);
    for (int i = sx + 1; i < ex; i++)
      F_MVputch(i, ey, h_line);
    for (int i = sy + 1; i < ey; i++)
      F_MVputch(sx, i, v_line);
    for (int i = sy + 1; i < ey; i++)
      F_MVputch(ex, i, v_line);
  }
}

void F_mbox(char *text) {
  int maxX, maxY;
  F_getmaxxy(maxX, maxY);

  F_move(maxX / 2, maxY / 2);
  F_printw("%s",maxX / 2,1, text);

  F_box(maxX / 2 - 1, maxY / 2 - 1, F_cursorx, F_cursory + 1, false);

  F_more();
}