#include "Const.h"
#include "Var.h"
#include <stdio.h>

int laud() {
  void *a = LaudVarVar(3.1);
  void *b = LaudConst_(3.9);
  void *c = LaudVarProduct(a, b);
  void *d = LaudVarProduct(c, c);
  void *e = LaudVarProduct(d, c);
  void *f = LaudVarProduct(e, c);
  void *g = LaudVarProduct(f, c);
  void *h = LaudVarProduct(g, c);
  void *i = LaudVarProduct(h, c);

  {
    void **ddx = LaudVarDerivativeMap();
    LaudVarDifferentiate(d, NULL, ddx);

    LaudVarDerivativeMapIterStart(ddx);
    void **dx;
    while ((dx = LaudVarDerivativeMapNext(ddx))) {
      printf("d\n\
\n\
x\n\
value: %g\n\
ref. count: %i\n\
nptr: %p\n\
\n\
dx\n\
value: %g\n\
ref. count: %i\n\
ptr: %p\n\
-----------------------------\n\
-----------------------------\n",
             LaudVarGetFloatValue(dx[0]), getReference(dx[0]), dx[0],
             LaudVarGetFloatValue(dx[1]), getReference(dx[1]), dx[1]);
      blip(dx[1]);
    }
    LaudVarDeleteDerivativeMap(ddx);
  }

  {
    void **ddx = LaudVarDerivativeMap();
    LaudVarDifferentiate(g, NULL, ddx);

    LaudVarDerivativeMapIterStart(ddx);
    void **dx;
    while ((dx = LaudVarDerivativeMapNext(ddx))) {
      printf("g\n\
\n\
x\n\
value: %g\n\
ref. count: %i\n\
nptr: %p\n\
\n\
dx\n\
value: %g\n\
ref. count: %i\n\
ptr: %p\n\
-----------------------------\n\
-----------------------------\n",
             LaudVarGetFloatValue(dx[0]), getReference(dx[0]), dx[0],
             LaudVarGetFloatValue(dx[1]), getReference(dx[1]), dx[1]);
      blip(dx[1]);
    }
    LaudVarDeleteDerivativeMap(ddx);
  }

  {
    void **ddx = LaudVarDerivativeMap();
    LaudVarDifferentiate(i, NULL, ddx);

    LaudVarDerivativeMapIterStart(ddx);
    void **dx;
    while ((dx = LaudVarDerivativeMapNext(ddx))) {
      printf("i\n\
\n\
x\n\
value: %g\n\
ref. count: %i\n\
nptr: %p\n\
\n\
dx\n\
value: %g\n\
ref. count: %i\n\
ptr: %p\n\
-----------------------------\n\
-----------------------------\n",
             LaudVarGetFloatValue(dx[0]), getReference(dx[0]), dx[0],
             LaudVarGetFloatValue(dx[1]), getReference(dx[1]), dx[1]);
      blip(dx[1]);
    }
    LaudVarDeleteDerivativeMap(ddx);
  }

  return 0;
}
