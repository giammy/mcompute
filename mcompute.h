#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <getopt.h>
#include <getopt.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>

/*

  Input parameters:

Matrices: 

matrix A rows: rowsA
matrix A cols: colsA
matrix B cols: colsB

Partitioning:

Matrix A, row partition, how many slides: rowAhowmany (-1 = all the needed)
Matrix A, row partition, slide size:      rowAsize
Matrix A, col partition, how many slides: colAhowmany (-1 = all the needed)
Matrix A, col partition, slide size:      colAsize
Matrix B, col partition, how many slides: colBhowmany (-1 = all the needed)
Matrix B, col partition, slide size:      colBsize

*/

#define DEBUG 1

#if DEBUG
#define WHEN_DEBUG(level, a) if (level <= debug) a
#else
#define WHEN_DEBUG(level, a) 
#endif

#define MATRIX_TYPE float

/* exit codes */
#define E_OK 0
#define E_NO_MEMORY -1

/* input args */
extern int rowsA;
extern int colsA;
extern int colsB;

extern int rowAhowmany;
extern int rowAsize   ;
extern int colAhowmany;
extern int colAsize   ;
extern int colBhowmany;
extern int colBsize   ;

extern int debug;

extern int sbatchSize;

extern char *prgName;

extern int compactOutput;

/* end input args */

extern MATRIX_TYPE *matrixA;
extern MATRIX_TYPE *matrixB;
extern MATRIX_TYPE *matrixC;

#define MATRIX(m,rtot,ctot,r,c) m[(r)*(ctot)+(c)] /* row major */

/* timing */
/* nested timing levels */
#define TIMING_NESTING 16

//extern time_t  t0[TIMING_NESTING]; 
       /* time_t is defined on <time.h> and <sys/types.h> as long */
extern clock_t c0[TIMING_NESTING]; 
       /* clock_t is defined on <time.h> and <sys/types.h> as int */
extern struct timeval tv[TIMING_NESTING];

extern int sbatchSize;

