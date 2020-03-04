int getMat(char* mat) {
  if (!strcmp("STONE",mat)) {
    return MAT_STONE;
  }
}

void readmat(char *buffer, int64_t *i, int64_t sizeb, int id) { //read a mat to mem
  skipcoments(buffer, i, sizeb);
  while (stringmatch(buffer, i, "", sizeb)) {
    skipcoments(buffer, i, sizeb);
    if (stringmatch(buffer, i, "]", sizeb)) {
      (*i)--;
      return;
    }
    if (stringmatch(buffer, i, "[DRAW", sizeb)) {

      printf ("got draw;");
      skipcoments(buffer, i, sizeb);
      matchcol(buffer, i, sizeb);

      if (!stringmatch(buffer, i, "\"", sizeb))
        configerror("DRAW is mising a string!!!");

      int pos = 0;
      for (int c = 0; c < 3; c++)
        if (buffer[*i] == '\\') {
          (*i)++;
          matTexture[id][pos] = buffer[(*i)];
          (*i)++;
          pos++;
        } else {
          matTexture[id][pos] = buffer[(*i)];
          (*i)++;
          pos++;
        }
      if (!stringmatch(buffer, i, "\"", sizeb))
        configerror("DRAW is mising a string!!!");
      skipcoments(buffer, i, sizeb);
      if (!stringmatch(buffer, i, "]", sizeb)) {
        configerror("bad DRAW: tag unclosed");
      }
    } else if (stringmatch(buffer, i, "[ORGANIC", sizeb)) {
      if (!stringmatch(buffer, i, "]", sizeb)) {
        configerror("bad ORGANIC: tag unclosed");
      }
      matDecompTo[id] = getMat("STONE");
      matDecompTemp[id] = 1180;
    } else {
      printf("BAD TAG: %s", &buffer[*i]);
      configerror("unrecognized tag in mat");
    }
    skipcoments(buffer, i, sizeb);
  }
}

void readraw(SDL_RWops *file, struct Raw *raw) {
  int64_t i = 0;
  int64_t sizeb = 0;
  int id = 0;
  char *buffer = NULL;
  sizeb = (file->size(file));
  buffer = malloc(1 + sizeb); //allock buffer and save size in size
  buffer[sizeb] = 0x00;
  SDL_RWseek(file, 0, RW_SEEK_SET);
  while ((SDL_RWread(file, &buffer[i], 1, 1))) {
    i++;
  } //read file into mem *buffer
  i = 0;
  skipcoments(buffer, &i, sizeb);
  while (buffer[i]) {
    skipcoments(buffer, &i, sizeb);
    if (stringmatch(buffer, &i, "[MAT", sizeb)) {

      printf("got mat;");
      matchcol(buffer, &i, sizeb);
      skipcoments(buffer, &i, sizeb);
      //printf("COMMENTS: %s", &buffer[i]);
      matchcol(buffer, &i, sizeb);
      readmat(buffer, &i, sizeb, id);
      skipcoments(buffer, &i, sizeb);

      if (!stringmatch(buffer, &i, "]", sizeb)) {
        configerror("bad MAT: tag unclosed");
      }
      id++;
    } else {
        printf("%s", &buffer[i]);
      configerror("unrecognized tag in raw");
    }
    skipcoments(buffer, &i, sizeb);
  }
  free(buffer);
}