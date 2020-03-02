void readraw(SDL_RWops *file, struct Raw *raw) {
  int64_t i = 0;
  int64_t sizeb = 0;
  char *buffer = NULL;
  //printf("file: %x size : %x",file,file->size);
  sizeb = (file->size(file));
  buffer = malloc(1 + sizeb); //allock buffer and save size in size
  buffer[sizeb] = 0x00;
  //    printf("loading objects..\n");
  SDL_RWseek(file, 0, RW_SEEK_SET);
  while ((SDL_RWread(file, &buffer[i], 1, 1))) {
    i++;
  } //read file into mem *buffer
  i = 0;
  //printf("rawbuffer: %s\n",buffer);
  while (i < sizeb) {
    if (stringmatch(buffer, &i, "[stoneMelt", sizeb)) {
      skipcoments(buffer, &i, sizeb);
      //printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
      matchcol(buffer, &i, sizeb);
      raw->stoneMelt = readint(buffer, &i, sizeb, 0, __INT16_MAX__);
      //F_printw("  stoneMelt: %f\n", raw->stoneMelt);
      //printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
      skipcoments(buffer, &i, sizeb);
      //printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
      if (!stringmatch(buffer, &i, "]", sizeb)) {
        configerror("bad savefile tag unclosed");
      }
    } else {
      if (stringmatch(buffer, &i, "[grassRegrow", sizeb)) {
        skipcoments(buffer, &i, sizeb);
        matchcol(buffer, &i, sizeb);

        raw->grassRegrow = readint(buffer, &i, sizeb, 0, __INT16_MAX__);
        //F_printw("  grassregrow: %d\n", 1, (int)(raw->grassRegrow));

        skipcoments(buffer, &i, sizeb);
        if (!stringmatch(buffer, &i, "]", sizeb)) {
          configerror("bad savefile tag unclosed");
        }
      } else {
        if (stringmatch(buffer, &i, "[seaLeval", sizeb)) {
          skipcoments(buffer, &i, sizeb);
          matchcol(buffer, &i, sizeb);

          raw->seaLeval = readfloat(buffer, &i, sizeb, 0, __INT16_MAX__);
          //F_printw("  seaLeval: %f\n", 1, raw->seaLeval);

          skipcoments(buffer, &i, sizeb);
          if (!stringmatch(buffer, &i, "]", sizeb)) {
            configerror("bad savefile tag unclosed");
          }
        } else {
          if (stringmatch(buffer, &i, "[MSPT", sizeb)) {
            skipcoments(buffer, &i, sizeb);
            matchcol(buffer, &i, sizeb);

            raw->MSPT = readint(buffer, &i, sizeb, 0, __INT16_MAX__);
            //F_printw("  MSPT: %x\n", 1, (int)(raw->MSPT));

            skipcoments(buffer, &i, sizeb);
            if (!stringmatch(buffer, &i, "]", sizeb)) {
              configerror("bad savefile tag unclosed");
            }
          } else {
            if (stringmatch(buffer, &i, "[placelavatemp", sizeb)) {
              skipcoments(buffer, &i, sizeb);
              matchcol(buffer, &i, sizeb);

              raw->lava = readint(buffer, &i, sizeb, 0, __INT16_MAX__);
              //F_printw("  lava: %x\n", 1, (int)(raw->lava));

              skipcoments(buffer, &i, sizeb);
              if (!stringmatch(buffer, &i, "]", sizeb)) {
                configerror("bad savefile tag unclosed");
              }
            } else {
              configerror("bad entry in raw");
            }
          }
        }
      }
    }
    skipcoments(buffer, &i, sizeb);
  }
  free(buffer);
}