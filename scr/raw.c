
void readraw(SDL_RWops* file ,struct Raw* raw) {
    int64_t i = 0;
    int64_t sizeb = 0;
    char* buffer = NULL;
    sizeb = (file -> size(file));
    buffer = malloc(1 + sizeb); //allock buffer and save size in size
    buffer[sizeb] = 0x00;

//    printf("loading objects..\n");

    while ((SDL_RWread(file,&buffer[i],1,1))) {i++;}//read file into mem *buffer
    //printf("rawbuffer: %s\n",buffer);

    i = 0;
    while(i<sizeb) {
        if (stringmatch(buffer, &i,"[stoneMelt", sizeb)) {
            skipcoments(buffer,&i,sizeb);
            //printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
            matchcol(buffer,&i,sizeb);

                raw -> stoneMelt = readfloat(buffer,&i,sizeb,-1,1);
                printf("stoneMelt: %f\n",raw -> stoneMelt);

            //printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
            skipcoments(buffer,&i,sizeb);
            //printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
            if(!stringmatch(buffer, &i,"]", sizeb)) {configerror("bad savefile tag unclosed");}
        } else{
            if (stringmatch(buffer,&i,"[grassRegrow", sizeb)) {
                skipcoments(buffer,&i,sizeb);
                matchcol(buffer,&i,sizeb);

                    raw -> grassRegrow = readint(buffer,&i,sizeb,0,__INT16_MAX__);
                    printf("grassregrow: %d\n",(int)(raw -> grassRegrow));

                skipcoments(buffer,&i,sizeb);
                if(!stringmatch(buffer, &i,"]", sizeb)) {configerror("bad savefile tag unclosed");}
            } else{
                if (stringmatch(buffer,&i,"[seaLeval", sizeb)) {
                    skipcoments(buffer,&i,sizeb);
                    matchcol(buffer,&i,sizeb);

                        raw -> seaLeval = readfloat(buffer,&i,sizeb,0,__INT16_MAX__);
                        printf("seaLeval: %f\n",raw -> seaLeval);

                    skipcoments(buffer,&i,sizeb);
                    if(!stringmatch(buffer, &i,"]", sizeb)) {configerror("bad savefile tag unclosed");}
             } else{
                    if (stringmatch(buffer,&i,"[MSPT", sizeb)) {
                        skipcoments(buffer,&i,sizeb);
                        matchcol(buffer,&i,sizeb);

                          raw -> MSPT = readint(buffer,&i,sizeb,0,__INT16_MAX__);
                          printf("MSPT: %x\n",(int)(raw -> MSPT));

                        skipcoments(buffer,&i,sizeb);
                        if(!stringmatch(buffer, &i,"]", sizeb)) {configerror("bad savefile tag unclosed");}
                    } else{
                        if (stringmatch(buffer,&i,"[placelavatemp", sizeb)) {
                        skipcoments(buffer,&i,sizeb);
                        matchcol(buffer,&i,sizeb);

                          raw -> lava = readint(buffer,&i,sizeb,0,__INT16_MAX__);
                          printf("lava: %x\n",(int)(raw -> lava));

                        skipcoments(buffer,&i,sizeb);
                        if(!stringmatch(buffer, &i,"]", sizeb)) {configerror("bad savefile tag unclosed");}
                    } else{
                         configerror("bad entry in raw");
                    }
                    }
                }
            }
        }
        skipcoments(buffer,&i,sizeb);
    }

    free(buffer);
}
