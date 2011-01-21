#include "mcompute.h"
#include "cblas.h"

int doInit(void) { return 0; }
int doEnd(void) { return 0; }

/*
 * BLAS routine - float
 */
int doProduct(MATRIX_TYPE *res, MATRIX_TYPE *a, int ra, int ca,
	      MATRIX_TYPE *b, int rb, int cb)
{
   enum CBLAS_ORDER order;
   enum CBLAS_TRANSPOSE transa;
   enum CBLAS_TRANSPOSE transb;

   assert(sizeof(MATRIX_TYPE) == sizeof(float));
   
   order = CblasRowMajor;
   transa = CblasNoTrans;
   transb = CblasNoTrans;
   
   cblas_sgemm(order,        transa,     transb, 
	       /*m*/ra,      /*n*/cb,    /*k*/ca,
	       /*alpha*/1.0,
	       /*a*/a,       /*lda*/ca,               // +1?    //ra,
	       /*b*/b,       /*ldb*/cb,               // +1?  rb,
	       /*beta*/0.0, 
	       /*c*/res,     /*ldc*/cb     );         // +1?  ra   );

   return E_OK;
}

