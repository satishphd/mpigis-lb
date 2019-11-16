#include "bufferManager/collectiveAttributes.h"


int CollectiveAttributes ::  getSendBufSize()
{
     return sendBufSize;
}
   
int CollectiveAttributes ::  getRecvBufSize()
{
   	 return recvBufSize;
}

int CollectiveAttrGeom ::  getSendBufSize()
{
     return sendBufSize;
}
   
int CollectiveAttrGeom ::  getRecvBufSize()
{
   	 return recvBufSize;
}

void CollectiveAttrGeom :: populateL1CollectiveAttributes(CollectiveInfo *sendBuffer, CollectiveInfo *recvBuffer, int nprocs)
{
   for(int p = 0; p< nprocs; p++) {
     
     sendBufSize += sendBuffer[p].layer1BufLen;
     
     sendCountsArr[p] = sendBuffer[p].layer1BufLen;
   }
    
   sdispls = prefixSum(sendCountsArr, nprocs);

   for(int p = 0; p< nprocs; p++) {
     
     recvBufSize += recvBuffer[p].layer1BufLen;
     
     recvCountArr[p] = recvBuffer[p].layer1BufLen;
   }
   
   rdispls = prefixSum( recvCountArr, nprocs);
}

void CollectiveAttrGeom :: populateL2CollectiveAttributes(CollectiveInfo *sendBuffer, CollectiveInfo *recvBuffer, int nprocs)
{
   for(int p = 0; p< nprocs; p++) {
     
     sendBufSize += sendBuffer[p].layer2BufLen;
     
     sendCountsArr[p] = sendBuffer[p].layer2BufLen;
   }
    
   sdispls = prefixSum(sendCountsArr, nprocs);

   for(int p = 0; p< nprocs; p++) {
     
     recvBufSize += recvBuffer[p].layer2BufLen;
     
     recvCountArr[p] = recvBuffer[p].layer2BufLen;
   }
   
   rdispls = prefixSum( recvCountArr, nprocs);
}


int* CollectiveAttrGeom :: prefixSum(int *orig, int length)
{
   int *prefixSum = (int*)malloc(length * sizeof(int));
   prefixSum[0] = 0;

   for(int i = 1; i<length; i++)
   {
	 prefixSum[i] = prefixSum[i-1] + orig[i-1];
   }
   return prefixSum;
 }