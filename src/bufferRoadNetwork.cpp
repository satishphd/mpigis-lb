#include "bufferManager/bufferRoadnetwork.h"

void BufferRoadNetwork :: init()
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
CollectiveAttributes* BufferRoadNetwork :: getAllToAllData()
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
           
       for(int cellId : *cells) {
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

map<int,list<RoadInfo*>* >* BufferRoadNetwork :: shuffleExchangeByCell()
{
      roadbox *envelopes = populateSendBuffer(m_attr, grid, args->numProcesses);
      pair<roadbox*, int>* myEnvelopes = exchangeEnvelopes(envelopes, m_attr);
      map<int,list<RoadInfo*>* > *envelopesByCellId= parseEnvelopesFromStruct(myEnvelopes);            

      return envelopesByCellId;
}

roadbox* BufferRoadNetwork :: populateSendBuffer(CollectiveAttributes *attr, Grid *grid, int numProcesses)
{
   int numShapes = attr->sendBufSize;
   //cout<<" # "<<numShapes<<" ";
   
   //struct bbox *envelopes = (struct bbox *)malloc(sizeof(struct bbox) * numShapes);
   roadbox *l1Envelopes = new roadbox[numShapes];
   
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

// serialization from envelope to struct bbox
//   long edgeId;
//   long startNodeId;
//   long endNodeId;
//   long wayId;
void BufferRoadNetwork :: packEnvelope( int cellId, list<Geometry*> *geoms,  roadbox *boxes, int *envCounter)
{
  
  for (list<Geometry*>::iterator it = geoms->begin(); it != geoms->end(); it++) {
    Geometry *geom = *it;
    const Envelope *env = geom->getEnvelopeInternal();
    //box = (struct bbox*)malloc(sizeof(struct bbox));
    boxes[*envCounter].x1 = env->getMinX();
    boxes[*envCounter].x2 = env->getMaxX();
    boxes[*envCounter].y1 = env->getMinY();
    boxes[*envCounter].y2 = env->getMaxY();
    
    boxes[*envCounter].cellId = cellId;
    
    RoadNode *info = (RoadNode*)geom->getUserData();
 
    boxes[*envCounter].edgeId = info->edgeId; 
    boxes[*envCounter].startNodeId = info->startNodeId; 
    boxes[*envCounter].endNodeId = info->endNodeId;     
    boxes[*envCounter].wayId = info->wayId; 
    
    *envCounter = *envCounter + 1;
  }
}

pair< roadbox*, int>* BufferRoadNetwork :: exchangeEnvelopes( roadbox *send_envelopes, CollectiveAttributes *attr) 
{
    SpatialTypes types;
    MPI_Datatype MPI_ROAD_BOX = types.createRoadBoxType();
    roadbox* recvBaseBuffer = new roadbox[attr->recvBufSize]; //(struct bbox *)malloc(attr->recvBufSize * sizeof(struct bbox*));
	
	MPI_Alltoallv(send_envelopes, attr->sendCountsArr, attr->sdispls, MPI_ROAD_BOX, 
	recvBaseBuffer, attr->recvCountArr, attr->rdispls, MPI_ROAD_BOX, MPI_COMM_WORLD);
	
	return new pair< roadbox*, int>(recvBaseBuffer, attr->recvBufSize);
	//printResults(recvBaseBuffer, attr->recvBufSize);
}


map<int,list<RoadInfo*>* >* BufferRoadNetwork :: parseEnvelopesFromStruct(pair<roadbox*, int>* recvBoxes)
{
    map<int,list<RoadInfo*>* > *envelopesByCellId = new map<int,list<RoadInfo*>* >();
   
    struct roadbox* boxes = recvBoxes->first;

    for(int i = 0; i<recvBoxes->second; i++) {
        //struct bbox box = boxes[i];
        int cellId = boxes[i].cellId;

      	Envelope *envelope = new Envelope(boxes[i].x1, boxes[i].x2, boxes[i].y1, boxes[i].y2);
        RoadInfo *info = new RoadInfo();
        info->env = envelope;
        info->edgeId = boxes[i].edgeId;
       
        if(envelopesByCellId->find(cellId) != envelopesByCellId->end()) {
          list<RoadInfo*>* envs = envelopesByCellId->at(cellId);
          envs->push_back(info);
       } 
       else {
          list<RoadInfo*>* envs = new list<RoadInfo*>();
          envs->push_back(info);
          envelopesByCellId->insert(pair<int, list<RoadInfo*>* >(cellId, envs));
       }
    }
    return envelopesByCellId;
}            


CollectiveAttributes* BufferRoadNetwork :: populateCollectiveAttributes(int *sendBuffer, int *recvBuffer, int nprocs)
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

int BufferRoadNetwork :: calcBufferSize(int *sendBuffer, int nprocs)
{
  int totalShapeCnt = 0;
  
  for(int counter = 0; counter < nprocs; counter++) {
      totalShapeCnt += sendBuffer[counter];
  }
  return totalShapeCnt;
}

int* BufferRoadNetwork :: prefixSum(int *orig, int length)
{
   int *prefixSum = (int*)malloc(length * sizeof(int));
   prefixSum[0] = 0;

   for(int i = 1; i<length; i++)
   {
	 prefixSum[i] = prefixSum[i-1] + orig[i-1];
   }
   return prefixSum;
}