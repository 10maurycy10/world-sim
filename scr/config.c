#include <SDL.h>

struct config{
    SDL_RWops* savefile;
    SDL_RWops* rawfile;
};


void configerror(char* test) {
    printf(test);
    _Exit(1);
}

char* getstrtag(char* file,int* ip , int size) {//gets a string and malock() it
    int size=0;
    char* string;
    char inch = 0;
    int i = 0;

    if (*ip>size) {configerror("bad str");}//get strlen
    if (file[*ip]!='"') {configerror("bad str no open '\"' ");}

    while (*ip<size) {
        if(file[*ip]='"') {break;}
        size++;
    }

    string = (char*)malloc(size+1);

    SDL_RWseek(file,*ip,0);

    SDL_RWread(file,&inch,1,1);
    while (i<size) {
        SDL_RWread(file,&inch,1,1);
        string[i]=inch;
        *ip++;
        i++;

    }
    string[size]=0x00;//null pad
    return(string); //return matched string
}

bool stringmatch(char* file , int* ip , char* str , int size) {
    int i = 0;
    char inch;
    while(str[i]) {//for all of instr
        if (){ //is EOF or not match
            return 1;
        }
        i++;
    }
    *ip = i + *ip;
    return 1;
}

void skipcoments(char* file , int* ip , int size) {
    while(*ip < size) {//read untill [ or ]
        if (file[*ip]=='[' || file[*ip]=='"' || file[*ip]==']') {break;}
        (*ip)++;
    }
}

void readconfig(SDL_RWops* file, struct config *configstruct) {
    int i = 0;
    int size = -1;
    char* strp;
    char* buffer = NULL;
    buffer = mallock(size = (file -> size(file))); //allock buffer and save size in size

    while ((SDL_RWread(file,buffer[i],1,1))) {i++;}//read file into mem *buffer

    i = 0;
    while(i<size) {
        if (stringmatch(buffer[i],"[savefile")) {
            skipcoments(buffer,&i);
        } else{
            if (stringmatch(buffer,&i,"[raw")) {
                skipcoments(buffer,&i);
            } else{
                config("bad entry");
            }
        }
    }


    free(buffer);
};

