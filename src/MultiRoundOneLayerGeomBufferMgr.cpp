#include "bufferManager/MultiRoundOneLayerGeomBufferMgr.h"

list<Geometry*>* MultiRoundOneLayerGeomBufferMgr :: shuffleExchange()
{
   //  MultiRoundOneLayerBufferMBR manager(this->strategy, this->grid, this->args);
//     pair< bbox*, int>* bBoxPair = manager.shuffleExchangeMBR();
//     
//     CollectiveAttrGeom* attrPair = getAllToAllData();
// 
// 	pair<char *,int>* recvBufPair = shuffleExchangeGeoms(attrPair);
// 
// 	//cout<<"shuffled"<<endl;
// 	
// 	WKTParser parser;
//     list<Geometry*>* geomListPair = parser.parseGeometryFromRecvBuffer(bBoxPair, recvBufPair);
//     
//     
//     return geomListPair;
    
    return NULL;
}

long MultiRoundOneLayerGeomBufferMgr :: countGeoms( map<int, list<Geometry*>* >* geomListPair)
{
     long total = 0;
     
     for( const auto& pair : *geomListPair )
     {
            list<Geometry*>* geoms = pair.second;
            total +=  geoms->size();      
     }
     return total;
}

map<int, list<Geometry*>* >* MultiRoundOneLayerGeomBufferMgr :: shuffleExchangeGrpByCell()
{
    MultiRoundOneLayerBufferMBR manager(this->strategy, this->grid, this->args);
    
    map<int, list<Geometry*>* >* global_GeomListPair = new map<int, list<Geometry*>* >();
    
    // create partitions of cells in a grid to do multi-round communication
    long totalGeoms = 0;
    
    for(int round = 0; round < this->numRounds; round++) {
       //printf("round = %d\n",round);
       //fflush(stdout);
       CollectiveAttributes *mbrAttr = manager.getAllToAllData(round, numRounds);
       //printf("After AlltoAll round = %d\n", round);
       
       pair< bbox*, int>* bBoxPair = manager.shuffleExchangeMBR(mbrAttr, round, numRounds);
       //printf("After shuffle MBR round = %d\n", round);
       
       CollectiveAttrGeom* geomAttr = getAllToAllData(round, &manager);
       
       setCollectiveAttributes(geomAttr);
	
	   pair<char *,int>* recvBufPair = shuffleExchangeGeoms(geomAttr, round, &manager);

	   //cout<<"shuffled"<<endl;
	
		WKTParser parser;
   		map<int, list<Geometry*>* >* geomListPair = parser.parseGeomFromRecvGrpByCellId(bBoxPair,
   																 recvBufPair);
        totalGeoms +=  countGeoms(geomListPair); 						
        										 
        delete []bBoxPair->first;
        
        global_GeomListPair->insert(geomListPair->begin(), geomListPair->end());
        
        //addToGlobalResultMap(geomListPair, global_GeomListPair);
        
    	delete []recvBufPair->first;
    
    }
    
    long global_output = 0;
    
    MPI_Allreduce(&totalGeoms, &global_output, 1, MPI_LONG, MPI_SUM, MPI_COMM_WORLD);
    cout<<global_output<<endl;
    // printf("Global Total Geoms %d \n", global_output);
    fflush(stdout);
    
    return global_GeomListPair;
} 

void MultiRoundOneLayerGeomBufferMgr :: addToGlobalResultMap( map<int, list<Geometry*>* >* localMap, 
												map<int, list<Geometry*>* >*globalMap)
{
	 // for( const auto& pair : *localMap ) {
// 	      int cellId = pair.first;
// 	      
// 	      if(false == globalMap->find(cellId)) {
// 	          
// 	          globalMap->insert(pair);
// 	      } else {
// 	         printf("Error Error Error Error Error Error  Error Error <<Cell Already Present !!! >>>>\n");
// 	      }
// 	      
// 	      
//      }
}

CollectiveAttrGeom* MultiRoundOneLayerGeomBufferMgr :: getAllToAllData(int currRound,
														MultiRoundOneLayerBufferMBR *mbrMgr)
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
       
       vector<int> *cellsForARound = mbrMgr->getCellsForARound(currRound, this->numRounds, cells);
           
       for(int cellId : *cellsForARound) {
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

pair<char *,int>* MultiRoundOneLayerGeomBufferMgr :: shuffleExchangeGeoms(
						CollectiveAttrGeom* l1Attr, int currRound, MultiRoundOneLayerBufferMBR *mbrMgr)
{
  
  char *sendL1Buffer = populateL1SendBuffer(l1Attr, grid, args->numProcesses, currRound, mbrMgr);
  
  assert(l1Attr->recvBufSize >= 0 && l1Attr->recvBufSize <= INT_MAX);
  
  char *recvL1Buffer = new char[l1Attr->recvBufSize]; 
  
  if(l1Attr->recvBufSize != 0 && recvL1Buffer == NULL) {
      cerr<<"New Failed before layer1 mpi_alltoallv l1Attr->recvBufSize "<<l1Attr->recvBufSize<<endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
      exit(0);
  }
  
  MPI_Alltoallv(sendL1Buffer, l1Attr->sendCountsArr, l1Attr->sdispls, MPI_CHAR, 
                recvL1Buffer, l1Attr->recvCountArr, l1Attr->rdispls, MPI_CHAR, MPI_COMM_WORLD);
  
  delete []sendL1Buffer;
  
  pair<char *,int> *pairL1 = new pair<char *,int>(recvL1Buffer, l1Attr->recvBufSize);
  
  return pairL1;
}

char* MultiRoundOneLayerGeomBufferMgr :: populateL1SendBuffer(CollectiveAttrGeom *attr, Grid *grid, 
			int numProcesses, int currRound, MultiRoundOneLayerBufferMBR *mbrMgr)
{
   int sendBufSize = attr->sendBufSize;

	   int sizeInMB = sendBufSize/(1024*1024);
	   if(sizeInMB >1000)
	       printf(" sendBufSize is  = %d \n",sizeInMB);
       fflush(stdout);
   		//return NULL;
   		
   
   assert(sendBufSize >= 0 && sendBufSize <= INT_MAX);
   
   char *sendBuf = new char[sendBufSize];
   
   if(sendBufSize != 0 && sendBuf == NULL) {
      cerr<<" New Failed in populateL1SendBuffer sendBufSize "<<sendBufSize<<endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
   }
        
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
   int buffCounter = 0;
   
   for(int pid = 0; pid < numProcesses; pid++) {
       
       vector<int> *cells = processToCellsMapping->at(pid);
       vector<int> *cellsForARound = mbrMgr->getCellsForARound(currRound, this->numRounds, cells);   
       
       for(int cellId : *cellsForARound) {
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
void MultiRoundOneLayerGeomBufferMgr :: packGeoms( list<Geometry*> *geoms, char *sendBuf, int *buffCounter)
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