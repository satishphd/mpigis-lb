#ifndef ALLOBJ_COLLECTIVES_H
#define ALLOBJ_COLLECTIVES_H

#include "../mpiTypes/mpitype.h"

class AllObjectsCollectiveAttr
{
   public:
   
   long real_sendBufSize;
   //long real_recvBufSize;
   
   int derived_sendBufSize;
   int derived_recvBufSize;
   
   // these data structures are based on derived sizes.
   int *sendCountsArr;
   int *sdispls;
   
   int *recvCountArr;
   int *rdispls;
   
   AllObjectsCollectiveAttr(int nprocs)
   {
      derived_sendBufSize = 0;
      derived_recvBufSize = 0; 
      
      real_sendBufSize = 0;
//       real_recvBufSize = 0; 
      
      sendCountsArr = (int*)malloc(nprocs * sizeof(int));
      sdispls = NULL;
      
      recvCountArr = (int*)malloc(nprocs * sizeof(int));
      rdispls = NULL;
   }
	
   long getSendBufSize();
   
   long getRecvBufSize();
   
   //MPI_Datatype MPI_GEOM;
   
 int* prefixSum(int *orig, int length);
 
 void populateL1CollectiveAttributes(AllObjCollectiveInfo *sendBuffer, AllObjCollectiveInfo *recvBuffer, int nprocs);

 void populateL2CollectiveAttributes(AllObjCollectiveInfo *sendBuffer, AllObjCollectiveInfo *recvBuffer, int nprocs);

};

#endif