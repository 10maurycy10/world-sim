#include <SDL.h>

struct Config {
    SDL_RWops* savefile;
    SDL_RWops* rawfile;
};


void configerror(char* test) {
    printf(test);
    _Exit(1);
}

char* getstrtag(char* file,int* ip , int size) {//gets a string and malock() it
    int strsize = 0;
    char* string;
    int i = 0;

    if (*ip>size) {configerror("bad str");}//get strlen
    if ((file[*ip])!='"') {
        configerror("bad str no open '\"' ");
    }
    (*ip)++;//skip opener "
    printf("config.getstrtag.pos : %d\n",((*ip)+strsize));
    while (((*ip)+strsize)<size) {
        printf("config.getstrtag.pos : %d\n",((*ip)+strsize));
        if(file[*ip+strsize]=='"') {break;}
        strsize++;
    }

    printf("config.getstrtag.size : %d \n",strsize);

    string = (char*)malloc(strsize+1);

    while (i<strsize) {
        string[i]=file[i+*ip];
        (*ip)++;
        i++;

    }
    (*ip)++;
    string[strsize]=0x00;//null pad
    return(string); //return matched string
}

int8_t stringmatch(char* file , int* ip , char* str , int size) {
    int i = 0;
    printf("config.strmatch: maching : \"%s\" , i: %d\n" , str , *ip);
    while(str[i]) {//for all of instr
        if (*ip > size) {
            return 0;
        }
        if (file[*ip + i] != str[i]) {return 0;}
        i++;
    }
    *ip+= i + 1;
    printf("config.strmatch: mached : \"%s\" , i: %d\n" , str , *ip);
    return 1;
}

void skipcoments(char* file , int* ip , int size) {
    while(*ip < size) {//read untill [ or ]
        if (file[*ip]=='[' || file[*ip]=='"' || file[*ip]==']') {break;}
        (*ip)++;
    }
}

void readconfig(SDL_RWops* file, struct Config *configstruct) {
    int i = 0;
    int size = -1;
    char* strp = NULL;
    char* buffer = NULL;
    buffer = malloc(1 + (size = (file -> size(file)))); //allock buffer and save size in size
    buffer[size] = 0x00;

    while ((SDL_RWread(file,&buffer[i],1,1))) {i++;}//read file into mem *buffer
    printf("savebuffer: %s\n",buffer);

    i = 0;
    while(i<size) {
        if (stringmatch(buffer, &i,"[savefile", size)) {
            skipcoments(buffer,&i,size);
            printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);

                strp = getstrtag(buffer,&i,size);
                configstruct -> savefile = SDL_RWFromFile( "./data/config.txt", "r+b" );
                free(strp);

            printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
            skipcoments(buffer,&i,size);
            printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
            if(!stringmatch(buffer, &i,"]", size)) {configerror("bad savefile tag unclosed");}
        } else{
            if (stringmatch(buffer,&i,"[raw", size)) {
                skipcoments(buffer,&i,size);

                    strp = getstrtag(buffer,&i,size);
                    configstruct -> rawfile = SDL_RWFromFile( "./data/config.txt", "r+b" );
                    free(strp);

                skipcoments(buffer,&i,size);
                if(!stringmatch(buffer, &i,"]", size)) {configerror("bad savefile tag unclosed");}
            } else{
                configerror("bad entry");
            }
        }
    }


    free(buffer);
};

