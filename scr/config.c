#include <SDL.h>

struct config{
    SDL_RWops* savefile;
};

void skipcoments(SDL_RWops *file, int *ip) {
    SDL_RWseek(file,*ip,0);
    //SDL_RWread (SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
}

void configerror(char* test) {
    printf(test);
    _Exit(1);
}

char* getstrtag(SDL_RWops* file,int* ip) {//gets a string and malock() it
    int size=0;
    int i = 0;
    char* string;
    char inch = 0;
    SDL_RWseek(file,*ip,0);
    if (SDL_RWread(file,&inch,1,1)==0 || inch!='"') {configerror("bad str");}
    //get strlen
    while (SDL_RWread(file,&inch,1,1)==1 && inch!='"') {size++;}
    string = (char*)malloc(size+1);
    SDL_RWseek(file,*ip,0);
    SDL_RWread(file,&inch,1,1);//skip past starting qouts
    printf("size : %x\n",size);
    while (i<size) {
        SDL_RWread(file,&inch,1,1);
        printf("read : %c\n",inch);
        string[i]=inch;
        i++;
    }
    string[size]=0;
    return(string);
}

bool stringmatch(char* str ,SDL_RWops* file , int* ip) {
    SDL_RWseek(file,*ip,0);
    int i = 0;
    char inch;
    while(str[i]) {//for all of instr
        if (SDL_RWread(file,&inch,1,1)==0 || inch!=str[i]){ //is EOF or not match
            SDL_RWseek(file,0,*ip);
            return 1;
        }
        printf("checked %d %c \n",i+*ip,inch);
        i++;
    }
    *ip = i;
    return 1;
}

void readconfig(SDL_RWops* file, struct config* configstruct) {
    int i = 0;
    char* str;
    skipcoments(file,&i);
    if (stringmatch("[savefile:",file,&i)) {
        str = getstrtag(file,&i);
        printf("savefile: %s \n",str);
        configstruct -> savefile=SDL_RWFromFile(str , "r+b" );
        free(str);
    } else {
        if (stringmatch("[raw:",file,&i)) {

        } else {
            configerror("bad entry in config file.\n");
        }
    }
};

