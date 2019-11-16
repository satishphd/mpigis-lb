#ifndef COLLECTIVES_H
#define COLLECTIVES_H

#include "../mpiTypes/mpitype.h"

class CollectiveAttributes
{
   
   public:
   int sendBufSize;
   int recvBufSize;
   
   int *sendCountsArr;
   int *sdispls;
   
   int *recvCountArr;
   int *rdispls;
   
   //MPI_Datatype MPI_SHAPE;
   int getSendBufSize();
   
   int getRecvBufSize();
};

class CollectiveAttrGeom
{
   public:
   
   int sendBufSize;
   int recvBufSize;
   
   int *sendCountsArr;
   int *sdispls;
   
   int *recvCountArr;
   int *rdispls;
   
   CollectiveAttrGeom(int nprocs)
   {
      sendBufSize = 0;
      recvBufSize = 0; 
      
      sendCountsArr = (int*)malloc(nprocs * sizeof(int));
      sdispls = NULL;
      
      recvCountArr = (int*)malloc(nprocs * sizeof(int));
      rdispls = NULL;
   }
	
   int getSendBufSize();
   
   int getRecvBufSize();
   
   //MPI_Datatype MPI_GEOM;
   
 int* prefixSum(int *orig, int length);
 
 void populateL1CollectiveAttributes(CollectiveInfo *sendBuffer, CollectiveInfo *recvBuffer, int nprocs);

 void populateL2CollectiveAttributes(CollectiveInfo *sendBuffer, CollectiveInfo *recvBuffer, int nprocs);

};

#endif