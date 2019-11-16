#include "bufferManager/AllObjectsCommBuffer_DerivedType.h"

list<Geometry*>* AllObjectsCommBuffer :: shuffleExchange()
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

long AllObjectsCommBuffer :: countGeoms( map<int, list<Geometry*>* >* geomListPair)
{
     long total = 0;
     
     for( const auto& pair : *geomListPair )
     {
            list<Geometry*>* geoms = pair.second;
            total +=  geoms->size();      
     }
     return total;
}

map<int, list<Geometry*>* >* AllObjectsCommBuffer :: shuffleExchangeGrpByCell()
{
    MultiRoundOneLayerBufferMBR manager(this->strategy, this->grid, this->args);
    
    map<int, list<Geometry*>* >* global_GeomListPair = new map<int, list<Geometry*>* >();
    
    // create partitions of cells in a grid to do multi-round communication
    long totalGeoms = 0;
    
    MPI_Datatype MPI_CHARs_NEW = create_MPI_CHARS_Type();
    
    for(int round = 0; round < this->numRounds; round++) {
       //printf("Id=%d Outer loop = %d\n",args->numProcesses, round);
       //fflush(stdout);
       
       CollectiveAttributes *mbrAttr = manager.getAllToAllData(round, numRounds);
       //printf("After AlltoAll round = %d\n", round);
       
       pair< bbox*, int>* bBoxPair = manager.shuffleExchangeMBR(mbrAttr, round, numRounds);
       printf("After shuffle MBR round = %d\n", round);
       
       AllObjectsCollectiveAttr* geomAttr = getAllToAllData(round, &manager);
       
       setCollectiveAttributes(geomAttr);
	
	   pair<char *,long>* recvBufPair = shuffleExchangeGeoms(geomAttr, round, &manager, MPI_CHARs_NEW);

	   //cout<<"shuffled"<<endl;
	    
		WKTParser parser;
   		map<int, list<Geometry*>* >* geomListPair = parser.parseGeomByProcess(mbrAttr, bBoxPair,
   										recvBufPair, geomAttr, args, PACKING_LEN);
   										
        //totalGeoms +=  countGeoms(geomListPair); 						
        
        global_GeomListPair->insert(geomListPair->begin(), geomListPair->end());
        
        
        delete []bBoxPair->first;
    	delete []recvBufPair->first;
    
    }
    //printf(" After AlltoAll round ");
   // long global_output = 0;
    
    //MPI_Allreduce(&totalGeoms, &global_output, 1, MPI_LONG, MPI_SUM, MPI_COMM_WORLD);
    //cout<<global_output<<endl;
    
               // printf("Global Total Geoms %d \n", global_output);
    //fflush(stdout);
    
    return global_GeomListPair;
} 

AllObjectsCollectiveAttr* AllObjectsCommBuffer :: getAllToAllData(int currRound,
														MultiRoundOneLayerBufferMBR *mbrMgr)
{
   int numProcesses = args->numProcesses;
   
   AllObjCollectiveInfo *sendBuffer = (AllObjCollectiveInfo *)malloc(numProcesses *sizeof(AllObjCollectiveInfo));
   AllObjCollectiveInfo *recvBuffer = (AllObjCollectiveInfo *)malloc(numProcesses *sizeof(AllObjCollectiveInfo));
   
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
 
   long realSendBufSizeForThisRound = 0;  
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
 
   for(int pid = 0; pid < numProcesses; pid++) {

	   long layer1VCntForP = 0;
       
       vector<int> *cells = processToCellsMapping->at(pid);
       
       vector<int> *cellsForARound = mbrMgr->getCellsForARound(currRound, this->numRounds, cells);
           
       for(int cellId : *cellsForARound) {
          Cell *cell = shapeInACell->at(cellId);
          
          list<Geometry*> *layerAGeom = cell->getLayerAGeom();
          
          for(list<Geometry*>::iterator it = layerAGeom->begin() ; it != layerAGeom->end(); ++it) {
             Geometry *geom = *it;
             ExtraInfo *info = (ExtraInfo*)geom->getUserData();
             layer1VCntForP += info->vertexStr.size();  // real length of character buffer
          }
       }
       //sendBuffer[pid].layer1BufLen = layer1VCntForP;
       realSendBufSizeForThisRound += layer1VCntForP;
       
       sendBuffer[pid].realLayer1BufLen = layer1VCntForP;
       
       sendBuffer[pid].derivedLayer1BufLen = ceil((double)layer1VCntForP/PACKING_LEN);
   }
   
   SpatialTypes types;
   MPI_Datatype MPI_ALL_COLL_INFO = types.createAllObjeCollectiveInfoType();
    
   MPI_Alltoall(sendBuffer, 1, MPI_ALL_COLL_INFO, recvBuffer, 1, MPI_ALL_COLL_INFO, MPI_COMM_WORLD);
   
   AllObjectsCollectiveAttr *l1Attributes = new AllObjectsCollectiveAttr(numProcesses);
   l1Attributes->populateL1CollectiveAttributes(sendBuffer, recvBuffer, numProcesses);
   l1Attributes->real_sendBufSize = realSendBufSizeForThisRound;
  
   return l1Attributes;
}

MPI_Datatype AllObjectsCommBuffer :: create_MPI_CHARS_Type()
{
   // To send more than 2GB of character data
   MPI_Datatype MPI_CHARs_NEW;
   MPI_Type_contiguous(PACKING_LEN, MPI_CHAR, &MPI_CHARs_NEW);
   MPI_Type_commit(&MPI_CHARs_NEW);
   
   return MPI_CHARs_NEW;
}

pair<char *,long>* AllObjectsCommBuffer :: shuffleExchangeGeoms(
						AllObjectsCollectiveAttr* l1Attr, int currRound, MultiRoundOneLayerBufferMBR *mbrMgr,
									MPI_Datatype MPI_CHARs_NEW)
{
  
  char *sendL1Buffer = populateL1SendBuffer(l1Attr, grid, args->numProcesses, currRound, mbrMgr);
  
  assert(l1Attr->derived_recvBufSize >= 0 && l1Attr->derived_recvBufSize <= INT_MAX);
  
  char *recvL1Buffer = new char[l1Attr->derived_recvBufSize * PACKING_LEN]; 
  
  if(l1Attr->derived_recvBufSize != 0 && recvL1Buffer == NULL) {
      cerr<<"New Failed before layer1 mpi_alltoallv l1Attr->recvBufSize "<<l1Attr->derived_recvBufSize<<endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
      exit(0);
  }
  //PACKING_LEN + 1;
  MPI_Alltoallv(sendL1Buffer, l1Attr->sendCountsArr, l1Attr->sdispls, MPI_CHARs_NEW, 
                recvL1Buffer, l1Attr->recvCountArr, l1Attr->rdispls, MPI_CHARs_NEW, MPI_COMM_WORLD);
  
  delete []sendL1Buffer;
  
  pair<char *,long> *pairL1 = new pair<char *,long>(recvL1Buffer, l1Attr->derived_recvBufSize * PACKING_LEN);
  
  return pairL1;
}

char* AllObjectsCommBuffer :: populateL1SendBuffer(AllObjectsCollectiveAttr *attr, Grid *grid, 
			int numProcesses, int currRound, MultiRoundOneLayerBufferMBR *mbrMgr)
{
   assert(attr->derived_sendBufSize >= 0  || 
                                     assert_msg(attr->derived_sendBufSize));
   long sendBufSize = ((long)attr->derived_sendBufSize) * PACKING_LEN;
//   printf("%ld %ld for round = %d \n",attr->real_sendBufSize, sendBufSize, currRound);
//   fflush(stdout);
/*
	   int sizeInMB = sendBufSize/(1024*1024);
	   if(sizeInMB >1000)
	       printf(" sendBufSize is  = %d \n",sizeInMB);
       fflush(stdout);
   		//return NULL;
*/ 		
   if(sendBufSize<0) {
     printf("Real send %ld padded %ld\n",attr->real_sendBufSize, sendBufSize);
     fflush(stdout);
   }
   assert(sendBufSize >= 0  || assert_msg(sendBufSize));

   // This is to pad sendBufSize, since it should be a multiple of PACKING_LEN
   //sendBufSize = sendBufSize + (PACKING_LEN - (sendBufSize % PACKING_LEN));

   char *sendBuf = new char[sendBufSize];
   
   if(sendBufSize != 0 && sendBuf == NULL) {
      cerr<<" New Failed in populateL1SendBuffer sendBufSize "<<sendBufSize<<endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
   }
        
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
   //long buffCounter = 0;
   
   for(int pid = 0; pid < numProcesses; pid++) {
       // reset to a unique offset in "sendBuf" for each process
	   long buffCounter = ((long)attr->sdispls[pid]) * PACKING_LEN;	 // offset per process
       
       long upperBound = ((long)attr->sdispls[pid+1]) * PACKING_LEN; // offset for next process
       
       if(pid == numProcesses-1) // last process
          upperBound = sendBufSize;	 // offset per process
       
       vector<int> *cells = processToCellsMapping->at(pid);
       vector<int> *cellsForARound = mbrMgr->getCellsForARound(currRound, this->numRounds, cells);   
       
       for(int cellId : *cellsForARound) {
          Cell *cell = shapeInACell->at(cellId);
          list<Geometry*> *geoms = cell->getLayerAGeom();
          
          packGeoms( geoms, sendBuf, &buffCounter);
          
          		// if(buffCounter > sendBufSize) {
//              		printf("%ld %ld \n",buffCounter, sendBufSize);
//              		fflush(stdout);
//              	}
             
          assert(buffCounter <= upperBound && buffCounter <= sendBufSize);
          
       }
   }
   		//printf("id=%d #%d \n",args->rank,currRound);
   		//fflush(stdout);
   return sendBuf;
}

// serialization from envelope to struct bbox
void AllObjectsCommBuffer :: packGeoms( list<Geometry*> *geoms, char *sendBuf, long *buffCounter)
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

// serialization from envelope to struct bbox
/* void MultiRoundOneLayerGeomBufferMgr :: packGeomsInDerivedType( list<Geometry*> *geoms, 
											char *sendBuf, int *buffCounter)
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
} */

