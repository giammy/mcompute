#include "mcompute.h"

int doInit(void) { return 0; }
int doEnd(void) { return 0; }

int doProduct(MATRIX_TYPE *res, MATRIX_TYPE *a, int ra, int ca,
	      MATRIX_TYPE *b, int rb, int cb)
{
  int i,j,k;
  MATRIX_TYPE sum;

  for (i=0; i<ra; i++) {
    for (j=0; j<cb; j++) {
      sum = 0;
      for (k=0; k<ca; k++)
	sum += MATRIX(a, ra, ca, i, k) * MATRIX(b, rb, cb, k, j);  
      MATRIX(res, ra, cb, i, j) = sum;
    }
  }
}
