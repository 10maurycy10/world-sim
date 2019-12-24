#include <SDL.h>

struct config{
    SDL_RWops* savefile;
};

char keys[][] = {
    "savefile"
}

void skip

void readconfig(SDL_RWops* file, struct config* configstruct) {
    int i;
    char inchar;
    while(SDL_RWwrite(file,&inchar,sizeof(char),1)) { //read until eof
        
    }
};

