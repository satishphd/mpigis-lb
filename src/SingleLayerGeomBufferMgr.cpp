#include "bufferManager/singleLayerGeomBufferMgr.h"

list<Geometry*>* SingleLayerGeomBufferMgr :: shuffleExchange()
{
    SingleLayerMBRBuffer manager(this->strategy, this->grid, this->args);
    pair< bbox*, int>* bBoxPair = manager.shuffleExchangeMBR();
    
    CollectiveAttrGeom* attrPair = getAllToAllData();

	pair<char *,int>* recvBufPair = shuffleExchangeGeoms(attrPair);

	//cout<<"shuffled"<<endl;
	
	WKTParser parser;
    list<Geometry*>* geomListPair = parser.parseGeometryFromRecvBuffer(bBoxPair, recvBufPair);
    
    return geomListPair;
}

int SingleLayerGeomBufferMgr :: countGeoms( map<int, list<Geometry*>* >* geomListPair)
{
     int total = 0;
     
     for( const auto& pair : *geomListPair )
     {
            list<Geometry*>* geoms = pair.second;
            total +=  geoms->size();      
     }
     return total;
}

map<int, list<Geometry*>* >* SingleLayerGeomBufferMgr :: shuffleExchangeGrpByCell()
{
    SingleLayerMBRBuffer manager(this->strategy, this->grid, this->args);
    pair< bbox*, int>* bBoxPair = manager.shuffleExchangeMBR();
    
    CollectiveAttrGeom* attr = getAllToAllData();
	
	setCollectiveAttributes(attr);
	
	pair<char *,int>* recvBufPair = shuffleExchangeGeoms(attr);

	//cout<<"shuffled"<<endl;
	
	WKTParser parser;
    map<int, list<Geometry*>* >* geomListPair = parser.parseGeomFromRecvGrpByCellId(bBoxPair, recvBufPair);
    
   //  int totalGeoms = countGeoms(geomListPair); 						
    
   // int global_output = 0;
    
   // MPI_Allreduce(&totalGeoms, &global_output, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
   // printf("Global Total Geoms %d \n", global_output);
   // fflush(stdout);
    return geomListPair;
} 

CollectiveAttrGeom* SingleLayerGeomBufferMgr :: getAllToAllData()
{
   int numProcesses = args->numProcesses;
   
   CollectiveInfo *sendBuffer = (CollectiveInfo *)malloc(numProcesses *sizeof(CollectiveInfo));
   CollectiveInfo *recvBuffer = (CollectiveInfo *)malloc(numProcesses *sizeof(CollectiveInfo));
   
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
 
   for(int pid = 0; pid < numProcesses; pid++) {

	   int layer1VCntForP = 0;
       
       vector<int> *cells = processToCellsMapping->at(pid);
           
       for(int cellId : *cells) {
          Cell *cell = shapeInACell->at(cellId);
          
          list<Geometry*> *layerAGeom = cell->getLayerAGeom();
          
          for(list<Geometry*>::iterator it = layerAGeom->begin() ; it != layerAGeom->end(); ++it) {
             Geometry *geom = *it;
             ExtraInfo *info = (ExtraInfo*)geom->getUserData();
             layer1VCntForP += info->vertexStr.size();  
          }
       }
       sendBuffer[pid].layer1BufLen = layer1VCntForP;
   }
   
   SpatialTypes types;
   MPI_Datatype MPI_COLL_INFO = types.createCollectiveInfoType();
    
   MPI_Alltoall(sendBuffer, 1, MPI_COLL_INFO, recvBuffer, 1, MPI_COLL_INFO, MPI_COMM_WORLD);
   
   CollectiveAttrGeom *l1Attributes = new CollectiveAttrGeom(numProcesses);
   l1Attributes->populateL1CollectiveAttributes(sendBuffer, recvBuffer, numProcesses);
  
   return l1Attributes;
}

//pair<map<int,vector<Geometry*>* > *, map<int,vector<Geometry*>* >* >*

pair<char *,int>* SingleLayerGeomBufferMgr :: shuffleExchangeGeoms(
													CollectiveAttrGeom* l1Attr)
{
  
  char *sendL1Buffer = populateL1SendBuffer(l1Attr, grid, args->numProcesses);
  
  assert(l1Attr->recvBufSize >= 0 && l1Attr->recvBufSize <= INT_MAX);
  
  char *recvL1Buffer = (char *)malloc(sizeof(char) * l1Attr->recvBufSize); 
  
  if(recvL1Buffer == NULL) {
      cerr<<"Malloc Failed before layer1 mpi_alltoallv l1Attr->recvBufSize "<<l1Attr->recvBufSize<<endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
      exit(0);
  }
  
  MPI_Alltoallv(sendL1Buffer, l1Attr->sendCountsArr, l1Attr->sdispls, MPI_CHAR, 
                recvL1Buffer, l1Attr->recvCountArr, l1Attr->rdispls, MPI_CHAR, MPI_COMM_WORLD);
  
  pair<char *,int> *pairL1 = new pair<char *,int>(recvL1Buffer, l1Attr->recvBufSize);
  
  return pairL1;
}

char* SingleLayerGeomBufferMgr :: populateL1SendBuffer(CollectiveAttrGeom *attr, Grid *grid, int numProcesses)
{
   int sendBufSize = attr->sendBufSize;

   assert(sendBufSize >= 0 && sendBufSize <= INT_MAX);

   char *sendBuf = (char *)malloc(sendBufSize * sizeof(char));
   
   if(sendBuf == NULL) {
      cerr<<"Malloc Failed in populateL1SendBuffer sendBufSize "<<sendBufSize<<endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
   }
        
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
   int buffCounter = 0;
   
   for(int pid = 0; pid < numProcesses; pid++) {
       
       vector<int> *cells = processToCellsMapping->at(pid);
           
       for(int cellId : *cells) {
          Cell *cell = shapeInACell->at(cellId);
          list<Geometry*> *geoms = cell->getLayerAGeom();
          
          packGeoms( geoms, sendBuf, &buffCounter);
          
          assert(buffCounter <= sendBufSize);
          
       }
   }
   //cout<<" # "<<envCounter<<" ";
   return sendBuf;
}

// serialization from envelope to struct bbox
void SingleLayerGeomBufferMgr :: packGeoms( list<Geometry*> *geoms, char *sendBuf, int *buffCounter)
{
   
    for(list<Geometry*>::iterator it = geoms->begin() ; it != geoms->end(); ++it) {
         Geometry *geom = *it;
         ExtraInfo *info = (ExtraInfo*)geom->getUserData();
         string wkt = info->vertexStr;
         const char *wktChArr = wkt.c_str();
         
         for(unsigned int i = 0; i<wkt.size(); i++) {
            sendBuf[*buffCounter] = wktChArr[i];
            *buffCounter = *buffCounter + 1;
         }
    }
}
