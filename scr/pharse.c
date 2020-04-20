#include <SDL.h>
#include "toolkit.h"

void skipTextUntilNonWhiteSpace(int* index, struct Str* text) {
    while (*index < text->size) {
        char c = text->data[*index];
        if (!(c == ' ' || c =='\n' || c =='\r'))
            return;
        (*index)++;
    }
}

void skipTextUntilTag(int* index, struct Str* text) {
    while (*index < text->size) {
        char c = text->data[*index];
        if (c == '[' || c==']')
            return;
        (*index)++;
    }
}

char* getID(int* index, struct Str* text) {
    char* data;

    int size = 0;
    int i = 0;

    skipTextUntilNonWhiteSpace(index, text);

    while (*index + i < text->size) {
        if (text->data[*index + i] == ':' || text->data[*index + i] == ']' || text->data[*index + i] == ' ') {
            break;
        }
        i++;
        size++;
    }
    i++;

    data = malloc((size + 1) * sizeof(char));

    SDL_memcpy(data, text->data + *index, (size - 0) * sizeof(char));
    data[size] = 0;

    *index += i - 1;

    return data;
}

bool TockenMatch(char* tag, int* i, struct Str* data) {
    int temp = *i;
    int ofset = 0;
    //printf("TockenMatch\n");
    while (tag[ofset]) { //sucsed if null is reached with out errors
        //printf("TockenMatch.loop\n '\\x%x' '\\x%x'",data -> data[temp],tag[ofset]);
        if (data -> data[temp] != tag[ofset]) {
            return 0; //un matched char
        }
        ofset++;
        if (!(temp < data->size)) {
            return 0; //fall on EOF
        }
        temp++;
    }
    (*i) = temp;
    return 1;
}

void checkTag(char* tag, int* i, struct Str* data, void (*callback)(struct Str*, int* i)) {
    if (TockenMatch(tag,i,data)) {
        callback(data,i);
    }
}

char *getstrtag(int *ip, struct Str* data) { //gets a string and malock() it
  int strsize = 0;
  char *string;
  int i = 0;

  if (*ip > data->size) {
    return 0;
  } //get strlen
  if ((data->data[*ip]) != '"') {
    return 0;
  }
  (*ip)++; //skip opener "
  //printf("config.getstrtag.pos : %d\n",((*ip)+strsize));
  while (((*ip) + strsize) < data->size) {
    // printf("config.getstrtag.pos : %d\n",((*ip)+strsize));
    if (data->data[*ip + strsize] == '"') {
      break;
    }
    strsize++;
  }

  //printf("config.getstrtag.data->size : %d \n",strsize);

  string = (char *)malloc(strsize + 1);

  while (i < strsize) {
    string[i] = data->data[*ip];
    (*ip)++;
    i++;
  }
  (*ip)++;
  string[strsize] = 0x00; //null pad
  return (string);        //return matched string
}

void readFile(char* filePath, void (*callback)(struct Str*)) {
    SDL_RWops* file = SDL_RWFromFile(filePath,"r+");
    if (file) {
        int size = file->size(file);
        struct Str* data = malloc(GET_V_SIZE(struct Str, char, size));
        SDL_RWread(file,data->data,sizeof(char),size);
        data -> size = size;
        callback(data);
        SDL_RWclose(file);
        free(data);
    }
}
