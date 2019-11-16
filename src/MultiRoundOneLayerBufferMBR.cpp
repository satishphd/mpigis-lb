#include "bufferManager/MultiRoundOneLayerBufferMBR.h"

vector<int>* MultiRoundOneLayerBufferMBR :: getCellsForARound(int currRound, int numRounds, 
																	vector<int>* entireCells)
{
    int cellsSize = entireCells->size();
    
    if(cellsSize < numRounds)
      cout<<" cellsSize "<<cellsSize<<" numRounds "<<numRounds;
      
    assert(cellsSize >= numRounds);
    
    int startIndex = currRound * (cellsSize/numRounds);
    int endIndex = (currRound+1) * (cellsSize/numRounds);

    if((numRounds-1) == currRound)
       endIndex = cellsSize;

    vector<int>* cellPartition = new vector<int>(); 

    for(int index = startIndex; index < endIndex; index++) {
       cellPartition->push_back(entireCells->at(index));
       //printf("%d ",entireCells->at(index));
    }   
    
    return cellPartition;
}

void MultiRoundOneLayerBufferMBR :: init()
{
      //pair<CollectiveAttributes*, CollectiveAttributes*>* attr = getAllToAllData();
      //printf("%d\n", args->rank);
      //printCollectiveAttributes(attr, args->numProcesses);
     
     // for(int round = 0; round < this->numRounds; round++) { 
//          m_attr = getAllToAllData(round);
//       }
      //return attr;
}

/* This is required for MPI_AllToAllv communication 
   because a process does not know how many it will receive from other fellow processes
*/
CollectiveAttributes* MultiRoundOneLayerBufferMBR :: getAllToAllData(int currRound, int numRounds)
{
   int numProcesses = args->numProcesses;
   
   int *sendBuffer = (int *)malloc(numProcesses *sizeof(int));
   int *recvBuffer = (int *)malloc(numProcesses *sizeof(int));
   
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
 
   for(int pid = 0; pid < numProcesses; pid++) {

	   int layer1CntForP = 0;
       
       vector<int> *cells = processToCellsMapping->at(pid);
       
       vector<int> *cellForARound = getCellsForARound(currRound, numRounds, cells);
           
       for(int cellId : *cellForARound) {
          Cell *cell = shapeInACell->at(cellId);
          layer1CntForP += cell->getLayerAGeom()->size();
       }
    
       sendBuffer[pid] = layer1CntForP;   
   }
   
   //printf(" Debug Before all to all \n");
   
   MPI_Alltoall(sendBuffer, 1, MPI_INT, recvBuffer, 1, MPI_INT, MPI_COMM_WORLD);
   
   //printf(" Debug After all to all \n");
   
   CollectiveAttributes *attributes = populateCollectiveAttributes(sendBuffer, recvBuffer, numProcesses);
   
   //pair<int,int> countPair = countShapes(recvBuffer, numProcesses);

   return attributes;
}

pair< bbox*, int>* MultiRoundOneLayerBufferMBR :: shuffleExchangeMBR( CollectiveAttributes *attr,
													int currRound, int numRounds)
{
      bbox *l1Envelopes = populateSendBuffer(attr, grid, args->numProcesses, currRound, numRounds);
      pair< bbox*, int>* myEnvelopesL1 = exchangeEnvelopes(l1Envelopes, attr);
      delete []l1Envelopes;
      //map<int,vector<GeomInfo*>* > *envelopesByCellId1= parseEnvelopesFromStruct(myEnvelopesL1);            

      return myEnvelopesL1;
}

map<int,list<GeomInfo*>* >* MultiRoundOneLayerBufferMBR :: shuffleExchangeByCell(CollectiveAttributes *attr, int currRound, int numRounds)
{
      struct bbox *l1Envelopes = populateSendBuffer(attr, grid, args->numProcesses, currRound, numRounds);
      pair<struct bbox*, int>* myEnvelopesL1 = exchangeEnvelopes(l1Envelopes, attr);
      map<int,list<GeomInfo*>* > *envelopesByCellId1= parseEnvelopesFromStruct(myEnvelopesL1);            

	  return envelopesByCellId1;
      //new pair<pair<struct bbox*, int>*, pair<struct bbox*, int>* >(myEnvelopesL1, myEnvelopesL2);
}

map<int,list<GeomInfo*>* >* MultiRoundOneLayerBufferMBR :: parseEnvelopesFromStruct(pair<struct bbox*, int>* recvBoxes)
{
    map<int,list<GeomInfo*>* > *envelopesByCellId = new map<int,list<GeomInfo*>* >();
   
    struct bbox* boxes = recvBoxes->first;

    for(int i = 0; i<recvBoxes->second; i++) {
        //struct bbox box = boxes[i];
        int cellId = boxes[i].cellId;

      	Envelope *envelope = new Envelope(boxes[i].x1, boxes[i].x2, boxes[i].y1, boxes[i].y2);
        GeomInfo *info = new GeomInfo();
        info->env = envelope;
        info->vertexStrLength = boxes[i].vertStrLen;
       
        if(envelopesByCellId->find(cellId) != envelopesByCellId->end()) {
          list<GeomInfo*>* envs = envelopesByCellId->at(cellId);
          envs->push_back(info);
       } 
       else {
          list<GeomInfo*>* envs = new list<GeomInfo*>();
          envs->push_back(info);
          envelopesByCellId->insert(pair<int, list<GeomInfo*>* >(cellId, envs));
       }
    }
    return envelopesByCellId;
}            


struct bbox* MultiRoundOneLayerBufferMBR :: populateSendBuffer(CollectiveAttributes *attr, Grid *grid, 
 											int numProcesses, int currRound, int numRounds)
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
       vector<int> *cellForARound = getCellsForARound(currRound, numRounds, cells);  
       
       for(int cellId : *cellForARound) {
          Cell *cell = shapeInACell->at(cellId);
          list<Geometry*> *geoms = cell->getLayerAGeom();
          
          packEnvelope(cellId, geoms, l1Envelopes, &envCounter);
          
          assert(envCounter <= numShapes);
       }
   }
   //cout<<" # "<<envCounter<<" ";
   return l1Envelopes;
}



pair< bbox*, int>* MultiRoundOneLayerBufferMBR :: exchangeEnvelopes( bbox *send_envelopes, CollectiveAttributes *attr) 
{
    SpatialTypes types;
    MPI_Datatype MPI_BOX = types.createBoxType();
    bbox* recvBaseBuffer = new bbox[attr->recvBufSize]; //(struct bbox *)malloc(attr->recvBufSize * sizeof(struct bbox*));
	
	MPI_Alltoallv(send_envelopes, attr->sendCountsArr, attr->sdispls, MPI_BOX, 
	recvBaseBuffer, attr->recvCountArr, attr->rdispls, MPI_BOX, MPI_COMM_WORLD);
	
	return new pair< bbox*, int>(recvBaseBuffer, attr->recvBufSize);
	//printResults(recvBaseBuffer, attr->recvBufSize);
}

// serialization from envelope to struct bbox
void MultiRoundOneLayerBufferMBR :: packEnvelope( int cellId, list<Geometry*> *geoms,  bbox *boxes, int *envCounter)
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

CollectiveAttributes* MultiRoundOneLayerBufferMBR :: populateCollectiveAttributes(int *sendBuffer, int *recvBuffer, int nprocs)
{
   CollectiveAttributes *attributes = new CollectiveAttributes();
   
   attributes->sendBufSize = calcBufferSize(sendBuffer, nprocs);

   attributes->sendCountsArr = sendBuffer;
    
   attributes->sdispls = prefixSum(sendBuffer, nprocs);

   attributes->recvCountArr = recvBuffer;
   attributes->recvBufSize = calcBufferSize(recvBuffer, nprocs);
   
   attributes->rdispls = prefixSum(recvBuffer, nprocs);
   
   return attributes;
}

int MultiRoundOneLayerBufferMBR :: calcBufferSize(int *sendBuffer, int nprocs)
{
  int totalShapeCnt = 0;
  
  for(int counter = 0; counter < nprocs; counter++) {
      totalShapeCnt += sendBuffer[counter];
  }
  return totalShapeCnt;
}

int* MultiRoundOneLayerBufferMBR :: prefixSum(int *orig, int length)
{
   int *prefixSum = (int*)malloc(length * sizeof(int));
   prefixSum[0] = 0;

   for(int i = 1; i<length; i++)
   {
	 prefixSum[i] = prefixSum[i-1] + orig[i-1];
   }
   return prefixSum;
}

 void MultiRoundOneLayerBufferMBR :: printCollectiveAttributes(CollectiveAttributes *attr, int nprocs)
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

