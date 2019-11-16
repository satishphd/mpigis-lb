#include "bufferManager/allobjectsCollectiveAttr.h"


long AllObjectsCollectiveAttr ::  getSendBufSize()
{
     return derived_sendBufSize;
}
   
long AllObjectsCollectiveAttr ::  getRecvBufSize()
{
   	 return derived_recvBufSize;
}

void AllObjectsCollectiveAttr :: populateL1CollectiveAttributes(AllObjCollectiveInfo *sendBuffer, 
															AllObjCollectiveInfo *recvBuffer, int nprocs)
{
   for(int p = 0; p < nprocs; p++) {
     
     derived_sendBufSize += sendBuffer[p].derivedLayer1BufLen;
     
     sendCountsArr[p] = sendBuffer[p].derivedLayer1BufLen;
   }
    
   sdispls = prefixSum(sendCountsArr, nprocs);

   for(int p = 0; p < nprocs; p++) {
     
     derived_recvBufSize += recvBuffer[p].derivedLayer1BufLen;
     
     recvCountArr[p] = recvBuffer[p].derivedLayer1BufLen;
   }
   
   rdispls = prefixSum( recvCountArr, nprocs);
}

void AllObjectsCollectiveAttr :: populateL2CollectiveAttributes(AllObjCollectiveInfo *sendBuffer, AllObjCollectiveInfo *recvBuffer, int nprocs)
{
   for(int p = 0; p< nprocs; p++) {
     
     derived_sendBufSize += sendBuffer[p].layer2BufLen;
     
     sendCountsArr[p] = sendBuffer[p].layer2BufLen;
   }
    
   sdispls = prefixSum(sendCountsArr, nprocs);

   for(int p = 0; p< nprocs; p++) {
     
     derived_recvBufSize += recvBuffer[p].layer2BufLen;
     
     recvCountArr[p] = recvBuffer[p].layer2BufLen;
   }
   
   rdispls = prefixSum( recvCountArr, nprocs);
}


int* AllObjectsCollectiveAttr :: prefixSum(int *orig, int length)
{
   int *prefixSum = (int*)malloc(length * sizeof(int));
   prefixSum[0] = 0;

   for(int i = 1; i<length; i++)
   {
	 prefixSum[i] = prefixSum[i-1] + orig[i-1];
   }
   return prefixSum;
 }