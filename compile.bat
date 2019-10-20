
echo off

echo compiling
gcc scr\main.c -o main.exe -Wall -lcurses2 > .\tmp
echo done compiling
cat .\tmp

rm .\tmp
