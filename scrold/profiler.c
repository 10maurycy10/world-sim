#include <string.h>
#include <time.h>
int P_ctag;
int times[256];
int P_time;

enum tags { T_init,
            T_spin,
            T_present,
            T_draw,
            T_input,
            T_tick };

void profile(int tag) {
  times[P_ctag] *= 29;
  times[P_ctag] += clock() - P_time;
  times[P_ctag] /= 30;
  P_time = clock();
  P_ctag = tag;
}
void printTag(char *str, int tag) {
  printf("%s : %d\n", str, times[tag]);
}
void pInit() {
  memset(times, 0, sizeof(int) * 256);
  P_time = clock();
  P_ctag = 0;
}