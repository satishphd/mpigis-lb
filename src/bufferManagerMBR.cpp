#include "bufferManager/bufferManagerMBR.h"


void MPI_BufferManager :: init()
{
      //pair<CollectiveAttributes*, CollectiveAttributes*>* attr = getAllToAllData();
      //printf("%d\n", args->rank);
      //printCollectiveAttributes(attr, args->numProcesses);
      
      m_attr = getAllToAllData();
      //return attr;
}

/* This is required for MPI_AllToAllv communication 
   because a process does not know how many it will receive from other fellow processes
*/
pair<CollectiveAttributes*, CollectiveAttributes*>* MPI_BufferManager :: getAllToAllData()
{
   int numProcesses = args->numProcesses;
   
   int *sendBuffer = (int *)malloc(2*numProcesses *sizeof(int));
   int *recvBuffer = (int *)malloc(2*numProcesses *sizeof(int));
   
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
 
   for(int pid = 0; pid < numProcesses; pid++) {

	   int layer1CntForP = 0;
   	   int layer2CntForP = 0;
       
       vector<int> *cells = processToCellsMapping->at(pid);
           
       for(int cellId : *cells) {
          Cell *cell = shapeInACell->at(cellId);
          layer1CntForP += cell->getLayerAGeom()->size();
          layer2CntForP += cell->getLayerBGeom()->size(); 
       }
       
       int pos = 2 * pid;
       sendBuffer[pos] = layer1CntForP;
       sendBuffer[pos + 1] = layer2CntForP;
   }
   
   //printf(" Debug Before all to all \n");
   
   MPI_Alltoall(sendBuffer, 2, MPI_INT, recvBuffer, 2, MPI_INT, MPI_COMM_WORLD);
   
   //printf(" Debug After all to all \n");
   
   CollectiveAttributes *l1Attributes = populateL1CollectiveAttributes(sendBuffer, recvBuffer, numProcesses);
   CollectiveAttributes *l2Attributes = populateL2CollectiveAttributes(sendBuffer, recvBuffer, numProcesses);
   //pair<int,int> countPair = countShapes(recvBuffer, numProcesses);

   return new pair<CollectiveAttributes*, CollectiveAttributes*>(l1Attributes, l2Attributes) ;
}

pair< pair< bbox*, int>* , pair< bbox*, int>* >* MPI_BufferManager :: shuffleExchangeMBR()
{
      bbox *l1Envelopes = populateL1SendBuffer4Box(m_attr->first, grid, args->numProcesses);
      pair< bbox*, int>* myEnvelopesL1 = exchangeEnvelopes(l1Envelopes, m_attr->first);
      //map<int,vector<GeomInfo*>* > *envelopesByCellId1= parseEnvelopesFromStruct(myEnvelopesL1);            

      bbox *l2Envelopes = populateL2SendBuffer4Box(m_attr->second, grid, args->numProcesses);
      pair< bbox*, int>* myEnvelopesL2 = exchangeEnvelopes(l2Envelopes, m_attr->second);
      //map<int,vector<GeomInfo*>* > *envelopesByCellId2 = parseEnvelopesFromStruct(myEnvelopesL2);            

      return new pair< pair< bbox*, int>* , pair< bbox*, int>* >(myEnvelopesL1, myEnvelopesL2); 
      //new pair<pair<struct bbox*, int>*, pair<struct bbox*, int>* >(myEnvelopesL1, myEnvelopesL2);
}

pair<map<int,vector<GeomInfo*>* > *, map<int,vector<GeomInfo*>* >* >* MPI_BufferManager :: shuffleExchangeMBRGroupByCellId()
{
      struct bbox *l1Envelopes = populateL1SendBuffer4Box(m_attr->first, grid, args->numProcesses);
      pair<struct bbox*, int>* myEnvelopesL1 = exchangeEnvelopes(l1Envelopes, m_attr->first);
      map<int,vector<GeomInfo*>* > *envelopesByCellId1= parseEnvelopesFromStruct(myEnvelopesL1);            

      struct bbox *l2Envelopes = populateL2SendBuffer4Box(m_attr->second, grid, args->numProcesses);
      pair<struct bbox*, int>* myEnvelopesL2 = exchangeEnvelopes(l2Envelopes, m_attr->second);
      map<int,vector<GeomInfo*>* > *envelopesByCellId2 = parseEnvelopesFromStruct(myEnvelopesL2);            

      return new pair<map<int,vector<GeomInfo*>* > *, map<int,vector<GeomInfo*>* > *>(envelopesByCellId1, envelopesByCellId2); 
      //new pair<pair<struct bbox*, int>*, pair<struct bbox*, int>* >(myEnvelopesL1, myEnvelopesL2);
}

map<int,vector<GeomInfo*>* >* MPI_BufferManager :: parseEnvelopesFromStruct(pair<struct bbox*, int>* recvBoxes)
{
    map<int,vector<GeomInfo*>* > *envelopesByCellId = new map<int,vector<GeomInfo*>* >();
   
    struct bbox* boxes = recvBoxes->first;

    for(int i = 0; i<recvBoxes->second; i++) {
        //struct bbox box = boxes[i];
        int cellId = boxes[i].cellId;

      	Envelope *envelope = new Envelope(boxes[i].x1, boxes[i].x2, boxes[i].y1, boxes[i].y2);
        GeomInfo *info = new GeomInfo();
        info->env = envelope;
        info->vertexStrLength = boxes[i].vertStrLen;
       
        if(envelopesByCellId->find(cellId) != envelopesByCellId->end()) {
          vector<GeomInfo*>* envs = envelopesByCellId->at(cellId);
          envs->push_back(info);
       } 
       else {
          vector<GeomInfo*>* envs = new vector<GeomInfo*>();
          envs->push_back(info);
          envelopesByCellId->insert(pair<int, vector<GeomInfo*>* >(cellId, envs));
       }
    }
    return envelopesByCellId;
}            


struct bbox* MPI_BufferManager :: populateL1SendBuffer4Box(CollectiveAttributes *attr, Grid *grid, int numProcesses)
{
   int numShapes = attr->sendBufSize;
   //cout<<" # "<<numShapes<<" ";
   
   //struct bbox *envelopes = (struct bbox *)malloc(sizeof(struct bbox) * numShapes);
   struct bbox *l1Envelopes = new struct bbox[numShapes];
   
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
   int envCounter = 0;
   
   for(int pid = 0; pid < numProcesses; pid++) {
       
       vector<int> *cells = processToCellsMapping->at(pid);
           
       for(int cellId : *cells) {
          Cell *cell = shapeInACell->at(cellId);
          list<Geometry*> *geoms = cell->getLayerAGeom();
          
          packEnvelope(cellId, geoms, l1Envelopes, &envCounter);
          
          assert(envCounter <= numShapes);
       }
   }
   //cout<<" # "<<envCounter<<" ";
   return l1Envelopes;
}

struct bbox* MPI_BufferManager :: populateL2SendBuffer4Box(CollectiveAttributes *attr, Grid *grid, int numProcesses)
{
   int numShapes = attr->sendBufSize;
   //cout<<" # "<<numShapes<<" ";
   
   //struct bbox *envelopes = (struct bbox *)malloc(sizeof(struct bbox) * numShapes);
   struct bbox *l2Envelopes = new struct bbox[numShapes];
   
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
   int envCounter = 0;
   
   for(int pid = 0; pid < numProcesses; pid++) {
       
       vector<int> *cells = processToCellsMapping->at(pid);
           
       for(int cellId : *cells) {
          Cell *cell = shapeInACell->at(cellId);
          list<Geometry*> *geoms = cell->getLayerBGeom();
          
          packEnvelope(cellId, geoms, l2Envelopes, &envCounter);
          
          assert(envCounter <= numShapes);
       }
   }
   //cout<<" # "<<envCounter<<" ";
   return l2Envelopes;
}


pair< bbox*, int>* MPI_BufferManager :: exchangeEnvelopes( bbox *send_envelopes, CollectiveAttributes *attr) 
{
    SpatialTypes types;
    MPI_Datatype MPI_BOX = types.createBoxType();
    bbox* recvBaseBuffer = new bbox[attr->recvBufSize]; //(struct bbox *)malloc(attr->recvBufSize * sizeof(struct bbox*));
	
	MPI_Alltoallv(send_envelopes, attr->sendCountsArr, attr->sdispls, MPI_BOX, 
	recvBaseBuffer, attr->recvCountArr, attr->rdispls, MPI_BOX, MPI_COMM_WORLD);
	
	return new pair< bbox*, int>(recvBaseBuffer, attr->recvBufSize);
	//printResults(recvBaseBuffer, attr->recvBufSize);
}

void MPI_BufferManager :: printResults( bbox* recvBuffer, int length) 
{
   if(recvBuffer!=nullptr)
     cout<<" # "<<recvBuffer[0].x1<<" # "<<recvBuffer[0].x2<<" # "<<recvBuffer[0].y1<<" # "<<recvBuffer[0].y1<<endl;
}

// serialization from envelope to struct bbox
void MPI_BufferManager :: packEnvelope( int cellId, list<Geometry*> *geoms,  bbox *boxes, int *envCounter)
{
  //struct bbox *box;
  //geos::io::WKTWriter wktWriter;
  
  for (list<Geometry*>::iterator it = geoms->begin(); it != geoms->end(); it++) {
    Geometry *geom = *it;
    const Envelope *env = geom->getEnvelopeInternal();
    //box = (struct bbox*)malloc(sizeof(struct bbox));
    boxes[*envCounter].x1 = env->getMinX();
    boxes[*envCounter].x2 = env->getMaxX();
    boxes[*envCounter].y1 = env->getMinY();
    boxes[*envCounter].y2 = env->getMaxY();
    boxes[*envCounter].cellId = cellId;
    
    ExtraInfo *info = (ExtraInfo*)geom->getUserData();
    //string str1 = wktWriter.write(geom);
    boxes[*envCounter].vertStrLen = info->vertexStr.size(); 
    
    *envCounter = *envCounter + 1;
  }
}

CollectiveAttributes* MPI_BufferManager :: populateL1CollectiveAttributes(int *sendBuffer, int *recvBuffer, int nprocs)
{
   CollectiveAttributes *attributes = new CollectiveAttributes();
   
   attributes->sendBufSize = calcSendBufferL1ShpCnt(sendBuffer, nprocs);

   int *sendL1Counts = (int*)malloc(nprocs * sizeof(int));
   int l1Cnter = 0;
   
   for(int bufCnter = 0; bufCnter< (2*nprocs); bufCnter = bufCnter+2) {
     sendL1Counts[l1Cnter] = sendBuffer[bufCnter];
     l1Cnter++; 
   }
   attributes->sendCountsArr = sendL1Counts;
    
   attributes->sdispls = prefixSum(sendL1Counts, nprocs);

   int totalReceiveL1Boxes;
   attributes->recvCountArr = popRecvL1Args(recvBuffer, nprocs, &totalReceiveL1Boxes);
   attributes->recvBufSize = totalReceiveL1Boxes;
   
   attributes->rdispls = prefixSum( attributes->recvCountArr, nprocs);
   
   return attributes;
}

CollectiveAttributes* MPI_BufferManager :: populateL2CollectiveAttributes(int *sendBuffer, int *recvBuffer, int nprocs)
{
   CollectiveAttributes *attributes = new CollectiveAttributes();
   
   attributes->sendBufSize = calcSendBufferL2ShpCnt(sendBuffer, nprocs);

   int *sendL2Counts = (int*)malloc(nprocs * sizeof(int));
   int l2Cnter = 0;
   
   for(int bufCnter = 1; bufCnter< (2*nprocs); bufCnter = bufCnter+2) {
     sendL2Counts[l2Cnter] = sendBuffer[bufCnter];
     l2Cnter++; 
   }
   attributes->sendCountsArr = sendL2Counts;
    
   attributes->sdispls = prefixSum(sendL2Counts, nprocs);

   int totalReceiveL2Boxes;
   attributes->recvCountArr = popRecvL2Args(recvBuffer, nprocs, &totalReceiveL2Boxes);
   attributes->recvBufSize = totalReceiveL2Boxes;
   
   attributes->rdispls = prefixSum( attributes->recvCountArr, nprocs);
   
   return attributes;
}

 void MPI_BufferManager :: printCollectiveAttributes(CollectiveAttributes *attr, int nprocs)
 {
  /*int sendBufSize; int recvBufSize; int *sendCountsArr; int *sdispls;
   int *recvCountArr; int *rdispls; */
   printf("%d %d \n", attr->sendBufSize, attr->recvBufSize);

   for(int i= 0; i<nprocs; i++) {
      //printf("%d ",attr->sendCountsArr[i]);
      //printf("%d ",attr->sdispls[i]);
      //printf("%d ",attr->recvCountArr[i]);
      printf("%d ",attr->rdispls[i]);
   }
   printf("\n");
   fflush(stdout);
   /*
   for(int i= 0; i<nprocs; i++) {
      printf("%d ",sdispls[i])
   }*/
 }   

int MPI_BufferManager :: calcSendBufferL1ShpCnt(int *sendBuffer, int nprocs)
{
  int totalShapeCnt = 0;
  
  for(int counter = 0; counter < (2*nprocs); counter = counter + 2) {
      totalShapeCnt += sendBuffer[counter];
  }
  return totalShapeCnt;
}

int MPI_BufferManager :: calcSendBufferL2ShpCnt(int *sendBuffer, int nprocs)
{
  int totalShapeCnt = 0;
  
  for(int counter = 1; counter < (2*nprocs); counter = counter + 2) {
      totalShapeCnt += sendBuffer[counter];
  }
  return totalShapeCnt;
}


int* MPI_BufferManager :: prefixSum(int *orig, int length)
{
   int *prefixSum = (int*)malloc(length * sizeof(int));
   prefixSum[0] = 0;

   for(int i = 1; i<length; i++)
   {
	 prefixSum[i] = prefixSum[i-1] + orig[i-1];
   }
   return prefixSum;
}

// I as a process want to know how many shapes I am going to receive from others
int* MPI_BufferManager :: popRecvL1Args(int *recvShpCntBuffer, int nprocs, int *aTotalReceiveBaseBoxes)
{
    int totalReceiveL1Boxes = 0;
	int j = 0; //clear j
	int *recvBaseBoxArr = (int *)malloc(nprocs * sizeof(int));
	for(int i=0;i<2*nprocs;i=i+2)
	{
		totalReceiveL1Boxes = totalReceiveL1Boxes + recvShpCntBuffer[i]; //recvBuffer size
		recvBaseBoxArr[j] = recvShpCntBuffer[i];
		j = j+1;
	}
	*aTotalReceiveBaseBoxes = totalReceiveL1Boxes;
	return recvBaseBoxArr;
}

int* MPI_BufferManager :: popRecvL2Args(int *recvShpCntBuffer, int nprocs, int *aTotalReceiveBaseBoxes)
{
    int totalReceiveL2Boxes = 0;
	int j= 0; //clear j
	int *recvOverlayBoxArr = (int *)malloc(nprocs * sizeof(int));
	for(int i=1;i<2*nprocs;i=i+2)
	{
		totalReceiveL2Boxes = totalReceiveL2Boxes + recvShpCntBuffer[i];   //recvBuffer size
		recvOverlayBoxArr[j] = recvShpCntBuffer[i];
		j = j+1;
	}
	*aTotalReceiveBaseBoxes = totalReceiveL2Boxes;
	return recvOverlayBoxArr;
}