#
# Test GEMM product
#

BLASDIR=/Users/giammy/Desktop/Dropbox/data-rem1/Repository/dakota-work/matrix_c/blas
BLASLIBDIR=$(BLASDIR)/CBLAS/lib
BLASLIBS=$(BLASLIBDIR)/LINUX/cblas_LINUX.a $(BLASLIBDIR)/blas_LINUX.a

ATLASDIR=/usr/local/atlas
ATLASLIBS=$(ATLASDIR)/lib/libatlas.a $(ATLASDIR)/lib/libcblas.a

all: mcompute-golden mcompute-cblas mcompute-atlas mcompute-cublas
	@echo "Done."

mcompute.o: mcompute.c
	gcc -c -O3 -DADD_ mcompute.c

# golden 
mcompute-golden: mcompute-golden.c mcompute.h mcompute.c
	gcc -O3 -o mcompute-golden mcompute.c mcompute-golden.c
	@echo "*** Done mcompute-golden"
	@echo

# cblas 3
mcompute-cblas: mcompute-cblas.c mcompute.h mcompute.o
	gcc -c -O3 -DADD_ -I$(BLASDIR)/CBLAS/src mcompute-cblas.c
	gfortran -o mcompute-cblas mcompute-cblas.o mcompute.o $(BLASLIBS)
	@echo "*** Done mcompute-cblas"
	@echo

# atlas
mcompute-atlas: mcompute-atlas.c mcompute.h mcompute.o
	gcc -c -O3 -I$(ATLASDIR)/include mcompute-atlas.c
	gcc -o mcompute-atlas mcompute-atlas.o mcompute.o $(ATLASLIBS)
	@echo "*** Done mcompute-atlas"
	@echo

mcompute-cublas: mcompute-cublas.c mcompute.c
	nvcc -o mcompute-cublas -lcublas mcompute-cublas.c mcompute.c
	@echo "*** Done mcompute-cublas"
	@echo

clean:
	rm -f *~
	rm -f *.o

clobber: clean
	rm -f mcompute-golden
	rm -f mcompute-cblas
	rm -f mcompute-atlas
	rm -f mcompute-cublas
