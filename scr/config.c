#include <SDL.h>

struct Config {
    SDL_RWops* savefile;
    SDL_RWops* rawfile;
};


void configerror(char* test) {
    printf(test);
    _Exit(1);
}

void skipspace (char* file , int* ip ,int size) { //you know what (who?)
    while (*ip<size){
        if (!(file[*ip]==' ' || file[*ip]=='\t'|| file[*ip]=='\n')) {break;}
        (*ip)++;
    }
}

void skipcoments(char* file , int* ip , int size) {
    while(*ip < size) {//read untill [ or ]
        if (file[*ip]=='[' || file[*ip]=='"' || file[*ip]==']' || file[*ip]==':') {break;}
        (*ip)++;
    }
}

double readfloat(char* file,int* i,int size,double min,double max) {
    skipspace(file,i,size);
    skipcoments(file,i,size);
    return min;
}

int long readint(char* file,int* i,int size,int min,int max) {

    int long val = 0;

    skipspace(file,i,size);
    while ((*i)<(size)) {
        //printf("config.read int scaned %d : %c\n",*i , file[*i]);
        switch (file[*i]){ //code rep...
            case '0':val=(0+val*10);
                break;
            case '1':val=(1+val*10);
                break;
            case '2':val=(2+val*10);
                break;
            case '3':val=(3+val*10);
                break;
            case '4':val=(4+val*10);
                break;
            case '5':val=(5+val*10);
                break;
            case '6':val=(6+val*10);
                break;
            case '7':val=(7+val*10);
                break;
            case '8':val=(8+val*10);
                break;
            case '9':val=(9+val*10);
                break;

            default:
                    if (val>max) {configerror("to big int");}
                    if (val<min) {configerror("to big int");}
                    return val;
                    break;
        }
        (*i)++;
    }
    if (val>max) {configerror("to big int");}
    if (val<min) {configerror("to big int");}
    return val;

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
    //printf("config.getstrtag.pos : %d\n",((*ip)+strsize));
    while (((*ip)+strsize)<size) {
       // printf("config.getstrtag.pos : %d\n",((*ip)+strsize));
        if(file[*ip+strsize]=='"') {break;}
        strsize++;
    }

    //printf("config.getstrtag.size : %d \n",strsize);

    string = (char*)malloc(strsize+1);

    while (i<strsize) {
        string[i]=file[*ip];
        (*ip)++;
        i++;

    }
    (*ip)++;
    string[strsize]=0x00;//null pad
    return(string); //return matched string
}

int8_t stringmatch(char* file , int* ip , char* str , int size) {
    int i = 0;
    //printf("config.strmatch: maching : \"%s\" , i: %d\n" , str , *ip);
    while(str[i]) {//for all of instr
        if (*ip > size) {
            return 0;
        }
        if (file[*ip + i] != str[i]) {return 0;}
        i++;
    }
    *ip += i;
    //printf("config.strmatch: mached : \"%s\" , i: %d\n" , str , *ip);
    return 1;
}



void matchcol(char* file , int* ip , int size) {
    if(!stringmatch(file,ip,":",size)){configerror("no col");}
}

void readconfig(SDL_RWops* file, struct Config *configstruct) {
    int i = 0;
    int size = -1;
    char* strp = NULL;
    char* buffer = NULL;
    buffer = malloc(1 + (size = (file -> size(file)))); //alloc buffer and save size in size
    buffer[size] = 0x00;

    printf("loading data...\n");

    while ((SDL_RWread(file,&buffer[i],1,1))) {i++;}//read file into mem *buffer
    //printf("savebuffer: %s\n",buffer);



    i = 0;
    while(i<size) {
        if (stringmatch(buffer, &i,"[savefile", size)) {
            skipcoments(buffer,&i,size);
            //printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
            matchcol(buffer,&i,size);
            skipcoments(buffer,&i,size);
                strp = getstrtag(buffer,&i,size);
                printf("savefile: %s\n",strp);
                configstruct -> savefile = SDL_RWFromFile(strp, "r+b" );
                free(strp);

            //printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
            skipcoments(buffer,&i,size);
            if(!stringmatch(buffer, &i,"]", size)) {configerror("bad savefile tag unclosed");}
        } else{
            if (stringmatch(buffer,&i,"[raw", size)) {
                skipcoments(buffer,&i,size);
                matchcol(buffer,&i,size);
                skipcoments(buffer,&i,size);
                    strp = getstrtag(buffer,&i,size);
                    printf("rawfile: %s\n",strp);
                    configstruct -> rawfile = SDL_RWFromFile(strp, "r+b" );
                    free(strp);

                skipcoments(buffer,&i,size);
                if(!stringmatch(buffer, &i,"]", size)) {configerror("bad savefile tag unclosed");}
            } else{
                configerror("bad entry");
            }
        }
        skipcoments(buffer,&i,size);
    }


    free(buffer);
};

