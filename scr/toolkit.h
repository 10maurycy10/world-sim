#ifndef HAVE_TOOLKIT
#define HAVE_TOOLKIT

#undef bool
#undef true
#undef fasle

#define bool _Bool
#define true 1
#define false 0
#define Null ((void*)0)
#define OFFSET(val,offset) (*(&val + offset))

#define AUTO_CASE(a,b) case a :;  \
                        {b;}          \
                        break;

#define assert(x) \
  do {            \
    if (x)        \
      _EXIT(1);   \
  } while (0)

struct Vect2 {
  double x;
  double y;
};

struct IntVect2 {
  int x;
  int y;
};

struct IntConstVect2 {
  Uint32 x;
  Uint32 y;
};

#endif