char* matNames[256];

int getMat(char* mat) { //DO NOT RUN OUSIDE OF READ RAW OR LOAD MATS
  for (int i = 0;i < 256;i++) {
    if (!strcmp(matNames[i],mat)) {
      return i;
    }
  }
  configerror("undfind mat");
  return 0;
}

int ScanColor(char* buffer,int* i) {
  skipspace(buffer,i,256);
  if (stringmatch(buffer,i,"ERROR",strlen(buffer))) {
    return(C_HIGH);
  }
  if (stringmatch(buffer,i,"TEXT",strlen(buffer))) {
    return(C_TEXT);
  }
  if (stringmatch(buffer,i,"RED",strlen(buffer))) {
    return(C_MAGMA);
  }
  if (stringmatch(buffer,i,"YELLOW",strlen(buffer))) {
    return(C_HIGH);
  }
  if (stringmatch(buffer,i,"GREEN",strlen(buffer))) {
    return(C_GRASS);
  }
  return(C_DIM);
}

void readmat(char *buffer, int *i, int sizeb, int id) { //read a mat to mem
  skipcoments(buffer, i, sizeb);
  while (stringmatch(buffer, i, "", sizeb)) {
    skipcoments(buffer, i, sizeb);
    if (stringmatch(buffer, i, "]", sizeb)) {
      (*i)--;
      return;
    }
    if (stringmatch(buffer, i, "[DRAW", sizeb)) {

      skipcoments(buffer, i, sizeb);
      matchcol(buffer, i, sizeb);
      skipcoments(buffer, i, sizeb);

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
    } else if (stringmatch(buffer, i, "[MELT", sizeb)) {
      skipcoments(buffer, i, sizeb);
      matchcol(buffer, i, sizeb);
      //skipcoments(buffer, i, sizeb);
      matMelt[id] = readint(buffer,i,sizeb,0,99999);
      skipcoments(buffer, i, sizeb);
      if (!stringmatch(buffer, i, "]", sizeb)) {
        configerror("bad MELT: tag unclosed");
      }
    } else if (stringmatch(buffer, i, "[COLOR", sizeb)) {
      for (int e = 0;e < 3;e++) {
        skipcoments(buffer,i,sizeb);
        matchcol(buffer,i,sizeb);
        matCol[id][e] = ScanColor(buffer,i);
      }
      if (!stringmatch(buffer, i, "]", sizeb)) {
        configerror("bad COLOR: tag unclosed");
      }
    } else {
      configerror("unrecognized tag in mat");
    }
    skipcoments(buffer, i, sizeb);
  }
}

void readraw(SDL_RWops *file, struct Raw *raw) {
  int i = 0;
  int sizeb = 0;
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

      matchcol(buffer, &i, sizeb);
      matNames[id] = getstrtag(buffer,&i,sizeb);
      matchcol(buffer, &i, sizeb);
      readmat(buffer, &i, sizeb, id);
      skipcoments(buffer, &i, sizeb);

      if (!stringmatch(buffer, &i, "]", sizeb)) {
        configerror("bad MAT: tag unclosed");
      }
      id++;

    } else {
      configerror("unrecognized tag in raw");
    }
    skipcoments(buffer, &i, sizeb);
  }
  free(buffer);
  loadMats();
  for (int i = 0;i < 256;i++)
    free(matNames[id]);
}