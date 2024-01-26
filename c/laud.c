#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <Ubject.h>

#include "Const.h"
#include "Var.h"

#ifdef _WIN32
__declspec(dllexport)
#endif
    int laud()
{

  printf("\n\n==============================================================\n"
         "%s\n"
         "==============================================================\n",
         __FILE__);
  float number = 3.1;
  void *a = laud_var(0, NULL, 1, &number);
  number = 3.9;
  void *b = laud_const(0, NULL, 1, &number);
  void *c = laud_product(a, b);
  void *d = laud_product(c, c);
  void *e = laud_product(d, c);
  void *f = laud_product(e, c);
  void *g = laud_product(f, c);
  void *h = laud_product(g, c);
  void *i = laud_product(h, c);

  {
    void **ddx = laud_create_derivative_map();
    laud_differentiate(d, NULL, ddx);

    laud_start_derivative_map_iteration(ddx);
    void **dx;
    while ((dx = laud_yield_derivative_map_value(ddx)))
    {
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
             laud_get_value(dx[0], 0), getReference(dx[0]), dx[0],
             laud_get_value(dx[1], 0), getReference(dx[1]), dx[1]);
      blip(dx[1]);
    }
    laud_delete_derivative_map(ddx);
  }

  {
    void **ddx = laud_create_derivative_map();
    laud_differentiate(g, NULL, ddx);

    laud_start_derivative_map_iteration(ddx);
    void **dx;
    while ((dx = laud_yield_derivative_map_value(ddx)))
    {
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
             laud_get_value(dx[0], 0), getReference(dx[0]), dx[0],
             laud_get_value(dx[1], 0), getReference(dx[1]), dx[1]);
      blip(dx[1]);
    }
    laud_delete_derivative_map(ddx);
  }

  {
    void **ddx = laud_create_derivative_map();
    laud_differentiate(i, NULL, ddx);

    laud_start_derivative_map_iteration(ddx);
    void **dx;
    while ((dx = laud_yield_derivative_map_value(ddx)))
    {
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
             laud_get_value(dx[0], 0), getReference(dx[0]), dx[0],
             laud_get_value(dx[1], 0), getReference(dx[1]), dx[1]);
      blip(dx[1]);
    }
    laud_delete_derivative_map(ddx);
  }

  printf("------------------------------------------------------\n");

  void *txt = laud_from_text(
      "/data/data/com.termux/files/home/storage/shared/Jay/Projects/"
      "laud/tmp.csv",
      ", ");
  char string[1024];
  printf("%s\n", laud_as_string(txt, string, 1024));
  laud_slice(txt, "2, :");

  printf("%s\n", laud_as_string(laud_slice(txt, "2, :"), string, 1024));
  return 0;
}
