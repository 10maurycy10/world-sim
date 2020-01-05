#include <stdio.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED
#include "game.c"
int main (int argc, char *argv[]) {
    int i = 0;
    char* cfile = "data/config.txt";
    char* rfile = NULL;
    if ((i + 1 ) < argc) {
        if (argv[i+1][0]=='-' && argv[i+1][1]=='c') {
            cfile = argv[i+2];
            i++;
            i++;
        } else {
            if (argv[i+1][0]=='-' && argv[i+1][1]=='r') {
                rfile = argv[i+2];
                i++;
                i++;
            }
        }

    }
    argv[0] = "STAWS by BuggyBug";
    SDL_RWops* config = SDL_RWFromFile( cfile, "r+b" );
    game(config,rfile);
    return 0;
    
}
