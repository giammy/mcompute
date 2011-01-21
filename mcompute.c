#include "mcompute.h"

/* input args */
int rowsA = -1;
int colsA = -1;
int colsB = -1;

int rowAhowmany = -1;
int rowAsize    = -1;
int colAhowmany = -1;
int colAsize    = -1;
int colBhowmany = -1;
int colBsize    = -1;

int debug = 0;

int sbatchSize = -1;

char *prgName = NULL;

int compactOutput = 0;

/* end input args */

MATRIX_TYPE *matrixA = NULL;
MATRIX_TYPE *matrixB = NULL;
MATRIX_TYPE *matrixC = NULL;

/* timing */
//time_t  t0[TIMING_NESTING]; 
//        /* time_t is defined on <time.h> and <sys/types.h> as long */
clock_t c0[TIMING_NESTING]; 
        /* clock_t is defined on <time.h> and <sys/types.h> as int */
int timingLevelCPU = 0; /* the current free level: lower level are busy */
int timingLevelWall = 0; /* the current free level: lower level are busy */
struct timeval tv[TIMING_NESTING];
int startCPUTime()
{
  if (timingLevelCPU >= TIMING_NESTING)
    return 0; /* no more space for timing */
  c0[timingLevelCPU++] = clock();
  return 0;
}
int startWallTime()
{
  if (timingLevelWall >= TIMING_NESTING)
    return 0; /* no more space for timing */

  //t0 = time(NULL);
  gettimeofday(&(tv[timingLevelWall++]), NULL); 
  return 0;
}

double endCPUTime()
{
  clock_t c1;
  c1 = clock();
  if (timingLevelCPU <= 0)
    return;
  --timingLevelCPU;
  return (((double)(c1 - c0[timingLevelCPU]))/((double)CLOCKS_PER_SEC)); 
         /* return seconds */
}
double endWallTime()
{
  //t1 = time(NULL);
  //return ((double)(t1-t0)); /* return seconds */

  int secs;
  struct timeval *tv0p;
  struct timeval tv1;
  gettimeofday(&tv1, NULL); 

  if (timingLevelWall <= 0)
    return;
  --timingLevelWall;

  tv0p = &(tv[timingLevelWall]);

  secs = tv1.tv_sec - tv0p->tv_sec;
  if (tv1.tv_usec < tv0p->tv_usec) {
    --secs;
    tv1.tv_usec += 1000000;
  }

  //GMYprintf("%e %d %d \n", ((double)(tv1.tv_usec - tv0p->tv_usec))*0.000001, tv1.tv_usec, tv0p->tv_usec);

  return (((double)secs) + ((double)(tv1.tv_usec - tv0p->tv_usec))*0.000001);
  /* return seconds */
}

struct option long_options[] = {
  {"rowsA",       1, 0, 0},
  {"colsA",       1, 0, 0},
  {"colsB",       1, 0, 0},
  {"rowAhowmany", 1, 0, 0},
  {"rowAsize",    1, 0, 0},
  {"colAhowmany", 1, 0, 0},
  {"colAsize",    1, 0, 0},
  {"colBhowmany", 1, 0, 0},
  {"colBsize",    1, 0, 0},
  {"debug",       1, 0, 0},
  {"help",        0, 0, 0},
  {"compactOutput", 0, 0, 0},
  {"sbatch",      1, 0, 0},
  {0, 0, 0, 0}
};


int usage(char *s)
{
  int i;
  printf("%s: usage:\n", s);
  for (i=0; long_options[i].name; i++) {
    printf("\t--%s\n", long_options[i].name);
  }

  printf("Example: ./mcompute --rowsA 100 --colsA 200 --colsB 300\n");
  printf("Example: ./mcompute --sbatch 100\n");
  printf("\n");
  printf("In sbatch mode, the input is the size of a square dense matrix.\n");
  printf("The output is: CPU and Wall time for computation\n");
  printf("and CPU and Wall time including initialization (in seconds)\n");
  exit(E_OK);
}

void free_all(void)
{
  WHEN_DEBUG(9,printf("in free_all\n"));
  if (prgName) free(prgName);
  if (matrixA) free(matrixA);
  if (matrixB) free(matrixB);
  if (matrixC) free(matrixC);
}

void exitWithFail(char *s)
{
  free_all();
  fprintf(stderr, "FAIL: %s\n", s);
  exit(-E_NO_MEMORY);
}

int parse_args(int argc, char *argv[])
{
  int c;
  
  int prgNameLength = strlen(argv[0]);
  if ((prgName = (char *)malloc(prgNameLength+1)) == NULL) {
    /* no memory to allocate a string ... something is really wrong */
    exitWithFail("parse_args: No memory");
  }
  assert(prgName);
  strncpy(prgName, argv[0], prgNameLength+1);

  while (1) {
    int option_index = 0;

    c = getopt_long (argc, argv, "a:b:c:d:e:f:g:i:l:m:h",
		     long_options, &option_index);
    if (c == -1)
      break;
    
    switch (c) {
    case 0:
      /* If this option set a flag, do nothing else now. */
      if (long_options[option_index].flag != 0)
	break;
      //printf ("option %s", long_options[option_index].name);
      //if (optarg)
      //  printf (" with arg %s", optarg);
      //printf ("\n");
      switch (option_index) {
      case 0:
	rowsA = atoi(optarg);
	break;
      case 1:
	colsA = atoi(optarg);
      break;
      case 2:
	colsB = atoi(optarg);
	break;
      case 3:
	rowAhowmany = atoi(optarg);
	break;
      case 4:
	rowAsize = atoi(optarg);
	break;
      case 5:
	colAhowmany = atoi(optarg);
	break;
      case 6:
	colAsize = atoi(optarg);
	break;
      case 7:
	colBhowmany = atoi(optarg);
	break;
      case '8':
	colBsize = atoi(optarg);
	break;
      case 9:
	debug = atoi(optarg);
	break;
      case 10:
	usage(prgName);
	break;
      case 11:
	compactOutput=1;
	break;
      case 12:
	sbatchSize = atoi(optarg);
	break;
      }
      break;
    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
    }
  }

  if (sbatchSize > 0) {
    /* enter batch mode: square matrix and compact output */
    compactOutput = 1;
    rowsA = sbatchSize;
    colsA = sbatchSize;
    colsB = sbatchSize;
  }

  return(E_OK);
}

int print_args(char *prgName) 
{
  if (debug>0) {
    printf("Arguments:\n");
    printf("prgName       = %s\n", prgName);	    
    printf("rowsA         = %d\n", rowsA);	    
    printf("colsA	  = %d\n", colsA);	    	    
    printf("colsB	  = %d\n", colsB);	    	    
    printf("rowAhowmany   = %d\n", rowAhowmany);	     
    printf("rowAsize      = %d\n", rowAsize);	        
    printf("colAhowmany   = %d\n", colAhowmany);	     
    printf("colAsize      = %d\n", colAsize);	        
    printf("colBhowmany   = %d\n", colBhowmany);	     
    printf("colBsize      = %d\n", colBsize);	        
    printf("debug         = %d\n", debug);	           
    printf("compactOutput = %d\n", compactOutput);	           
    printf("sbatchSize    = %d\n", sbatchSize);	           
  }

  if (compactOutput && sbatchSize < 0) {
    printf("# prgName ");	    
    printf("rowsA ");	    
    printf("colsA ");	    	    
    printf("colsB ");	    	    
    printf("rowAhowmany ");	     
    printf("rowAsize ");	        
    printf("colAhowmany ");	     
    printf("colAsize ");	        
    printf("colBhowmany ");	     
    printf("colBsize ");	        
    printf("debug ");	           
    printf("compactOutput ");	           
    printf("sbatchSize\n");	           

    printf("# %s ", prgName);	    
    printf("%d ", rowsA);	    
    printf("%d ", colsA);	    	    
    printf("%d ", colsB);	    	    
    printf("%d ", rowAhowmany);	     
    printf("%d ", rowAsize);	        
    printf("%d ", colAhowmany);	     
    printf("%d ", colAsize);	        
    printf("%d ", colBhowmany);	     
    printf("%d ", colBsize);	        
    printf("%d ", debug);	           
    printf("%d ", compactOutput);	           
    printf("%d\n", sbatchSize);	           
  }

  return(E_OK);
}

int allocate_matrix(MATRIX_TYPE **matrix, int rows, int cols)
{
  int sz = rows*cols;
  MATRIX_TYPE *p;

  if (*matrix) {
    WHEN_DEBUG(9,printf("free matrix in allocate_matrix\n"));
    free(*matrix);
  }

  p = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE)*sz);

  if (p == NULL) {
    exitWithFail("allocate_matrix: no memory");
  }

  *matrix = (MATRIX_TYPE *)p;
  return (E_OK);
}

int initialize_matrix(MATRIX_TYPE **matrix, int rows, int cols)
{
  int i;
  MATRIX_TYPE *p;
  int sz = rows*cols;
  srand((unsigned)time(NULL));

  allocate_matrix(matrix, rows, cols);

  p = (*matrix);
  assert(p);

  for (i=0;i<sz; i++)
    *p++ = ((MATRIX_TYPE)rand())/((MATRIX_TYPE)(RAND_MAX)); 

  return 0;
}


int main(int argc, char *argv[])
{
  double elapsedCPU1, elapsedWall1;
  double elapsedCPU2, elapsedWall2;
  parse_args(argc, argv);
  print_args(prgName);

  if (rowsA <=0 || colsA <= 0 || colsB <= 0) 
    usage(prgName);

  startWallTime();
  startCPUTime();

  initialize_matrix(&matrixA, rowsA, colsA);
  initialize_matrix(&matrixB, colsA, colsB);
  allocate_matrix(&matrixC, rowsA, colsB);

  doInit();

  startWallTime();
  startCPUTime();
  doProduct(matrixC, matrixA, rowsA, colsA, matrixB, colsA, colsB);
  elapsedCPU1 = endCPUTime();
  elapsedWall1 = endWallTime();

  doEnd();

  elapsedCPU2 = endCPUTime();
  elapsedWall2 = endWallTime();

  if (compactOutput) {
    if (sbatchSize > 0)
      printf("%d ", sbatchSize);
    printf("%es ", elapsedCPU1);
    printf("%es ", elapsedWall1);

    printf("%es ", elapsedCPU2);
    printf("%es\n", elapsedWall2);

  } else {
    if (sbatchSize > 0)
      printf("sbatchSize = %d\n", sbatchSize);
    printf("Elapsed CPU  = %es\n", elapsedCPU1);
    printf("Elapsed WALL = %es\n", elapsedWall1);
    printf("Elapsed CPU  = %es (including init)\n", elapsedCPU2);
    printf("Elapsed WALL = %es (including init)\n", elapsedWall2);
  }
}
