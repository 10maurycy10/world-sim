#include <SDL.h>

struct F_Color {
    uint16_t red,blue,green;
};

int F_x_size = 40;
int F_y_size = 80;
int F_cursorx = 1;
int F_cursory = 1;
SDL_Surface* F_font;
SDL_Texture* fontT;
SDL_Renderer* gRenderer;

SDL_Surface* gScreenSurface;
#define F_getmaxxy(x,y) do {x = gScreenSurface -> w / F_x_size; y = gScreenSurface -> h / F_y_size;} while (0)

#define F_catch(x) do {int y; if ((y = (x))) {printf("ERROR: %s\n",SDL_GetError()); _Exit(1);}} while (0);

//xxxx xxxx CCCC CCCC xxxx xxxx xxxx xxxxx
//* C : color pair
#define F_I_COLOR_SHIFT 24

#define F_COLOR_PAIR(x) (x << F_I_COLOR_SHIFT)
uint64_t F_I_attr;
#define F_ATTR(x) F_I_attr=x;

SDL_Renderer* gRenderer = NULL;
#define NO_COL 256
struct F_Color F_colors[NO_COL][2];

SDL_Surface* loadSurface(char* path) {
    SDL_Surface* loadedSurface = SDL_LoadBMP(path);
	if( loadedSurface == NULL ) {
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

SDL_Window* window = NULL;

void F_load(char* font) {
    SDL_Surface* s = loadSurface(font);

    int charH = (s -> h) / 16;
    int charW = (s -> pitch) / 16;

    F_x_size = charW;
    F_y_size = charH;

    //Uint32 colorKey = SDL_MapRGB(s -> format, 0xff, 0, 0);
    //s = SDL_ConvertSurface(s, gScreenSurface -> format, 0);
    //F_catch(SDL_SetColorKey(s,SDL_TRUE,colorKey));

    fontT = SDL_CreateTextureFromSurface( gRenderer, s );
    SDL_FreeSurface(s);
    //F_catch(SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0x00, 0xFF ));
}

void F_init() {

    F_colors[0][0].red = 0xFF; // set up COLOR
    F_colors[0][0].blue = 0xFF;
    F_colors[0][0].green = 0xFF;
    F_colors[0][1].red = 0x00;
    F_colors[0][1].blue = 0x00;
    F_colors[0][1].green = 0x00;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    window = SDL_CreateWindow("[world simulator]", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_SHOWN );
    gScreenSurface = SDL_GetWindowSurface(window);
    //SDL_FillRect( gScreenSurface, NULL, SDL_MapRGB( gScreenSurface->format, 0x00, 0x00, 0x00 ) ); // RGB
    SDL_UpdateWindowSurface(window);
    gRenderer = SDL_CreateRenderer(window, -1 , SDL_RENDERER_ACCELERATED );
    SDL_RenderClear(gRenderer);
}

void F_end() {
    SDL_DestroyTexture(fontT);
    SDL_DestroyRenderer(gRenderer);
    SDL_Quit();
}

void F_refresh() {
    SDL_RenderCopy(gRenderer,fontT, NULL , NULL);
    //SDL_RenderPresent(gRenderer);
    F_catch(SDL_UpdateWindowSurface(window));
    F_catch(SDL_RenderClear(gRenderer));
}

void F_move(int x, int y)  {
    F_cursorx = x;
    F_cursory = y;
}

void F_clear() {
    //clear();
    F_catch(SDL_RenderClear(gRenderer));
    F_move(0,0);
}

bool F_gete(char block,SDL_Event* e) {
    do {
        while(SDL_PollEvent(e)==0 && block) {;}
        if( e -> type == SDL_QUIT ) {
            gend();
            F_end();
            _Exit(0);
        } else if (e -> type == SDL_KEYDOWN) {
            return true;
        }
    } while (block);
    return false;
}

void F_more() {
    F_refresh();
    SDL_Event e;
    F_gete(true,&e);
}

void F_MVputch(int x, int y, int c) {

    int pixX = F_x_size * x;
    int pixY = F_y_size * y;

    int fontX = c%16;
    int fontY = c/16;

    SDL_Rect scr = { fontX, fontY, F_x_size, F_y_size};
    SDL_Rect dst = { pixX, pixY, F_x_size, F_y_size};

    F_catch(SDL_RenderCopy(gRenderer,fontT, &scr , &dst));
    //SDL_RenderCopy(gRenderer,fontT, NULL,NULL);

}   

void F_putch(char c) {
    F_MVputch(F_cursorx,F_cursory,c);
    F_cursorx++;
}

void F_printw(char* x ,int n, ...) {

    va_list a;
    va_start(a, n);

    for (uint16_t i = 0; x[i];) {
        if(x[i] == '%') {
            i++;
            switch (x[i]) {
                case 's' :;
                    char* str = va_arg(a, char*);
                    for (int e = 0; str[e]; e++) {
                        F_putch(str[e]);
                    }
                break;
                case 'd' :;
                    int in = va_arg(a, int);
                    //printw("%d",in);
                break;
                case 'x' :;
                    int i = va_arg(a, int);
                    //printw("%x",i);
                break;
            }
            i++;
        } else {
            F_putch(x[i]);
            i++;
        }
    }
}
