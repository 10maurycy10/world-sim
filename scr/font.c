#include <SDL.h>

struct F_Color {
    uint16_t red,blue,green;
};

int F_x_size = 40;
int F_y_size = 80;

SDL_Surface* gScreenSurface;
#define F_getmaxxy(x,y) do {x = gScreenSurface -> w / F_x_size; y = gScreenSurface -> h / F_y_size;} while (0)



//xxxx xxxx CCCC CCCC xxxx xxxx xxxx xxxxx
//* C : color pair
#define F_I_COLOR_SHIFT 24

#define F_COLOR_PAIR(x) (x << F_I_COLOR_SHIFT)
uint64_t F_I_attr;
#define F_ATTR(x) F_I_attr=x;

#define NO_COL 256
struct F_Color F_colors[NO_COL][2];

SDL_Surface* loadSurface(char* path) {
    SDL_Surface* optimizedSurface = NULL;
    SDL_Surface* loadedSurface = SDL_LoadBMP(path);
	if( loadedSurface == NULL ) {
		printf("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
        _Exit(1);
	} else
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
    return optimizedSurface;
}

void F_initpair(int no, int fr, int fb, int fg, int br, int bb, int bg) {

    F_colors[no][0].red = fr;
    F_colors[no][0].blue = fb;
    F_colors[no][0].green = fg;

    F_colors[no][1].red = br;
    F_colors[no][1].blue = bb;
    F_colors[no][1].green = bg;
}

SDL_Window* window = NULL;


void F_init() {

    F_colors[0][0].red = 0xFF; // set up COLOR
    F_colors[0][0].blue = 0xFF;
    F_colors[0][0].green = 0xFF;
    F_colors[0][1].red = 0x00;
    F_colors[0][1].blue = 0x00;
    F_colors[0][1].green = 0x00;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("world simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 40 * 10, 80 * 10, SDL_WINDOW_SHOWN );
    gScreenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect( gScreenSurface, NULL, SDL_MapRGB( gScreenSurface->format, 0x00, 0x00, 0x00 ) ); // RGB

    initscr();
    noecho();
    curs_set(0);
    //start_color();
    raw();
}

void F_end() {
    endwin();
    SDL_Quit();
    clear();
}

void F_refresh() {
    refresh();
    SDL_UpdateWindowSurface(window);
}

void F_more() {

    F_refresh();

    SDL_Event e;

    while (1) {
        while(SDL_PollEvent(&e)==0 ) {;}
        if( e.type == SDL_QUIT ) {
            F_end();
            _Exit(0);
        } else if (e.type == SDL_KEYDOWN) {return;}
    }
}
void F_clear() {
    clear();
}
void F_move(int x, int y)  {
    move(y,x);
}
void F_MVputch(int x, int y, int c) {
    mvaddch(y,x,c);
}

void F_putch(char c) {
    addch(c);
}


void F_printw(char* x , ...) {

    va_list a;
    va_start(a, 256);

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
                    printw("%d",in);
                break;
                case 'x' :;
                    int i = va_arg(a, int);
                    printw("%x",i);
                break;
            }
            i++;
        } else {
            F_putch(x[i]);
            i++;
        }
        
    }
}