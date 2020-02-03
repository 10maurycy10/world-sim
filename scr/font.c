#include <SDL.h>

struct F_Color {
    uint16_t red,blue,green;
};

struct F_Color F_colors[256][2];
void F_initpair(int no; int fr; int fb; int fg; int br; int bb; int bg;) {}

void F_init() {

    F_colors[0][0]

   initscr();
   start_color();
   raw();
}