#include<mpi.h>
#include "Types.h"

//#include "ST_Intersect.h"



//void run_kernel(int x);
// void ST_Intersect(long bPolNum, long oPolNum, coord_t* baseCoords, coord_t* overlayCoords, int* bVNum, int* oVNum,
//  long *bVPSNum, long *oVPSNum, long long* seqMBR, long long* seqOMBR, float *seqMBR2, float* seqOMBR2);



//int ST_Intersect(long bPolNum, long oPolNum, coord_t* baseCoords, coord_t* overlayCoords,
//                 int* bVNum, int* oVNum, long *bVPSNum, long *oVPSNum,
//                 mbr_t* seqMBR, mbr_t* seqOMBR, coord_t *seqMBR2, coord_t* seqOMBR2, int*);
int ST_Intersect(long, long, coord_t*, coord_t*,
                 int*, int*, long*, long*,
                 mbr_t*, mbr_t*, coord_t*, coord_t*, int*);
 
int main(int argc, char *argv[])
{
int rank, size;
MPI_Init (&argc, &argv); /* starts MPI */
MPI_Comm_rank (MPI_COMM_WORLD, &rank); /* get current process id */
MPI_Comm_size (MPI_COMM_WORLD, &size); /* get number of processes */
//run_kernel(5);

float *b_coord;
float *o_coord;
int *b_Vnum;
int *o_Vnum;

long *bPrefixSum;
long *oPrefixSum;
mbr_t* bmbr2;
mbr_t* ombr2;
coord_t *bmbr;
coord_t *ombr;
int *joinPairs;

// void ST_Intersect(long bPolNum, long oPolNum, coord_t* baseCoords, coord_t* overlayCoords,
// int* bVNum, int* oVNum, 
// long *bVPSNum, long *oVPSNum, 
// mbr_t* seqMBR, mbr_t* seqOMBR, 
// coord_t *seqMBR2, coord_t* seqOMBR2){

ST_Intersect(100, 200, b_coord, o_coord, 
		    b_Vnum, o_Vnum,
		    bPrefixSum, oPrefixSum,
		    bmbr2, ombr2,
		    bmbr, ombr, joinPairs);	

MPI_Finalize();
return 0;
}
