struct Raw {
    int stoneMelt;
    int grassRegrow;
    int seaLeval;
};

void readraw(SDL_RWops* file ,struct Raw* target) {
    int i = 0;
    int size = -1;
    char* buffer = NULL;
    buffer = malloc(1 + (size = (file -> size(file)))); //allock buffer and save size in size
    buffer[size] = 0x00;

    while ((SDL_RWread(file,&buffer[i],1,1))) {i++;}//read file into mem *buffer
    printf("rawbuffer: %s\n",buffer);

    i = 0;
    while(i<size) {
        if (stringmatch(buffer, &i,"[stoneMelt", size)) {
            skipcoments(buffer,&i,size);
            printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);

                target -> stoneMelt = ;

            printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
            skipcoments(buffer,&i,size);
            printf("config.readconfig.savefileread: i : %d , curch: '%c' \n",i,buffer[i]);
            if(!stringmatch(buffer, &i,"]", size)) {configerror("bad savefile tag unclosed");}
        } else{
            if (stringmatch(buffer,&i,"[grassRegrow", size)) {
                skipcoments(buffer,&i,size);

                    target -> grassRegrow = 0;

                skipcoments(buffer,&i,size);
                if(!stringmatch(buffer, &i,"]", size)) {configerror("bad savefile tag unclosed");}
            } else{
                configerror("bad entry");
            }
        }
    }


    free(buffer);
}
