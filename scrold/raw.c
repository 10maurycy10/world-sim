char *matNames[256];
int matNum = 0;

int getMat(char *mat) {
  for (int i = 0; i < 256 && matNames[i]; i++) {
    if (!strcmp(matNames[i], mat)) {
      return i;
    }
  }
  configerror("undfind mat");
  return 0;
}

int ScanColor(char *buffer, int *i) {
  skipspace(buffer, i, 256);
  if (stringmatch(buffer, i, "ERROR", strlen(buffer),false)) {
    return (C_ERROR);
  }
  if (stringmatch(buffer, i, "TEXT", strlen(buffer),false)) {
    return (C_TEXT);
  }
  if (stringmatch(buffer, i, "DIM", strlen(buffer),false)) {
    return (C_DIM);
  }
  if (stringmatch(buffer, i, "RED", strlen(buffer),false)) {
    return (C_MAGMA);
  }
  if (stringmatch(buffer, i, "YELLOW", strlen(buffer),false)) {
    return (C_HIGH);
  }
  if (stringmatch(buffer, i, "GREEN", strlen(buffer),false)) {
    return (C_GRASS);
  }
  if (stringmatch(buffer, i, "BLUE", strlen(buffer),false)) {
    return (C_WATER);
  }
  return (C_ERROR);
}

void readmat(char *buffer, int *i, int sizeb, int id, bool init) { //read a mat to mem

  if (init) {

    gMats[id].mossy = 0;
    gMats[id].matgrownd = 0;
    gMats[id].matMelt = __INT_MAX__;
    gMats[id].matDecompTemp = __INT_MAX__;
    gMats[id].matDecompTo = -1;
    gMats[id].matVoid = 0;
    gMats[id].mat_name = NULL;
  }

  skipcoments(buffer, i, sizeb);
  while (stringmatch(buffer, i, "", sizeb,true)) {
    skipcoments(buffer, i, sizeb);
    if (stringmatch(buffer, i, "]", sizeb,false)) {
      (*i)--;
      return;
    }
    if (stringmatch(buffer, i, "[DRAW", sizeb,false)) {

      skipcoments(buffer, i, sizeb);
      matchcol(buffer, i, sizeb);
      skipcoments(buffer, i, sizeb);

      if (!stringmatch(buffer, i, "\"", sizeb,true))
        configerror("DRAW is mising a string!!!");

      int pos = 0;
      for (int c = 0; c < 3; c++)
        if (buffer[*i] == '\\') {
          (*i)++;
          gMats[id].matTexture[pos] = buffer[(*i)];
          (*i)++;
          pos++;
        } else {
          gMats[id].matTexture[pos] = buffer[(*i)];
          (*i)++;
          pos++;
        }
      if (!stringmatch(buffer, i, "\"", sizeb,true))
        configerror("DRAW is mising a string!!!");
      skipcoments(buffer, i, sizeb);
      if (!stringmatch(buffer, i, "]", sizeb,false)) {
        configerror("bad DRAW: tag unclosed");
      }
    } else if (stringmatch(buffer, i, "[ORGANIC", sizeb,false)) {
      if (!stringmatch(buffer, i, "]", sizeb,false)) {
        configerror("bad ORGANIC: tag unclosed");
      }
      gMats[id].matDecompTo = getMat("STONE");
      gMats[id].matDecompTemp = 1180;
    } else if (stringmatch(buffer, i, "[MELT", sizeb,false)) {
      skipcoments(buffer, i, sizeb);
      matchcol(buffer, i, sizeb);
      //skipcoments(buffer, i, sizeb);
      gMats[id].matMelt = readint(buffer, i, sizeb, 0, 99999);
      skipcoments(buffer, i, sizeb);
      if (!stringmatch(buffer, i, "]", sizeb,false)) {
        configerror("bad MELT: tag unclosed");
      }
    } else if (stringmatch(buffer, i, "[MOSS", sizeb,false)) {
      if (!stringmatch(buffer, i, "]", sizeb,false)) {
        configerror("bad MOSS: tag unclosed");
      }
      gMats[id].mossy = 1;
    } else if (stringmatch(buffer, i, "[COLOR", sizeb,false)) {
      for (int e = 0; e < 3; e++) {
        skipcoments(buffer, i, sizeb);
        matchcol(buffer, i, sizeb);
        gMats[id].matCol[e] = ScanColor(buffer, i);
      }
      if (!stringmatch(buffer, i, "]", sizeb,false)) {
        configerror("bad COLOR: tag unclosed");
      }
    } else if (stringmatch(buffer, i, "[VOID", sizeb,false)) {
      gMats[id].matVoid = 1;
      if (!stringmatch(buffer, i, "]", sizeb,false)) {
        configerror("bad VOID: tag unclosed");
      }
    } else if (stringmatch(buffer, i, "[FLOOR", sizeb,false)) {
      if (!stringmatch(buffer, i, "]", sizeb,false)) {
        configerror("bad ORGANIC: tag unclosed");
      }
      gMats[id].matgrownd = 1;
    } else if (stringmatch(buffer, i, "[DESC", sizeb,false)) {
      matchcol(buffer, i, sizeb);
      gMats[id].mat_name = getstrtag(buffer, i, __INT_MAX__);
      if (!stringmatch(buffer, i, "]", sizeb,false)) {
        configerror("bad DESK: tag unclosed");
      }

    } else {
      printf("%s\n", &(buffer[*i]));
      configerror("unrecognized tag in mat");
    }
    skipcoments(buffer, i, sizeb);
  }
}

void readraw(SDL_RWops *file, struct Raw *raw) {
  matNum = 0;
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
    if (stringmatch(buffer, &i, "[MAT_DEF", sizeb,false)) {

      matchcol(buffer, &i, sizeb);
      matNames[id] = getstrtag(buffer, &i, sizeb);
      matchcol(buffer, &i, sizeb);
      readmat(buffer, &i, sizeb, id, true);
      skipcoments(buffer, &i, sizeb);

      if (!stringmatch(buffer, &i, "]", sizeb,false)) {
        configerror("bad MAT: tag unclosed");
      }
      id++;
      matNum++;

    } else if (stringmatch(buffer, &i, "[MAT_SELECT", sizeb,false)) {

      char *name = NULL;

      matchcol(buffer, &i, sizeb);
      name = getstrtag(buffer, &i, sizeb);
      matchcol(buffer, &i, sizeb);
      readmat(buffer, &i, sizeb, getMat(name), false);
      skipcoments(buffer, &i, sizeb);

      if (!stringmatch(buffer, &i, "]", sizeb,false)) {
        configerror("bad MAT SELECT: tag unclosed");
      }

    } else {
      printf("%s", &buffer[i]);
      configerror("unrecognized tag in raw");
    }
    skipcoments(buffer, &i, sizeb);
  }
  free(buffer);
  loadMats();
}