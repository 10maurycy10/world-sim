#ifndef HAVE_TOOLKIT
#define HAVE_TOOLKIT

#undef bool
#undef true
#undef fasle

#define bool _Bool
#define true 1
#define false 0

#define assert(x) \
  do {            \
    if (x)        \
      _EXIT(1);   \
  } while (0)

int toolkit_main(int, char **);
int main(int x, char **y) {
  toolkit_main(x, y);
}

#define main toolkit_main
#endif