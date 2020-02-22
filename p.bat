echo off
gcc scr\main.c -o pb.exe -lcurses2 scr/SDL2.lib -Wall -Wextra -pedantic -DP -s -O3 && echo BUILD OK
strip pb.exe && echo STIP OK