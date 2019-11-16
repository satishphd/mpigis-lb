#include "bufferManager/bufferManagerGeoms.h"

pair<list<Geometry*>*, list<Geometry*>* >* BufferManagerForGeoms :: shuffleExchange()
{
    MPI_BufferManager manager(this->strategy, this->grid, this->args);
    pair< pair< bbox*, int>* , pair< bbox*, int>* >* bBoxPair = manager.shuffleExchangeMBR();
    
    pair<CollectiveAttrGeom*, CollectiveAttrGeom*>* attrPair = getAllToAllData();

	pair<pair<char *,int>*, pair<char *,int>* >*recvBufPair = shuffleExchangeGeoms(attrPair);

	//cout<<"shuffled"<<endl;
	
	WKTParser parser;
    pair<list<Geometry*>*, list<Geometry*>* >* geomListPair = parser.parseGeometryFromRecvBuffer(bBoxPair, recvBufPair);
    
    return geomListPair;
}

pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >* BufferManagerForGeoms :: shuffleExchangeGrpByCell()
{
    MPI_BufferManager manager(this->strategy, this->grid, this->args);
    pair< pair< bbox*, int>* , pair< bbox*, int>* >* bBoxPair = manager.shuffleExchangeMBR();
    
    pair<CollectiveAttrGeom*, CollectiveAttrGeom*>* attrPair = getAllToAllData();

	pair<pair<char *,int>*, pair<char *,int>* >*recvBufPair = shuffleExchangeGeoms(attrPair);

	//cout<<"shuffled"<<endl;
	
	WKTParser parser;
    pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >*geomListPair = 
    														parser.parseGeomFromRecvGrpByCellId(bBoxPair, recvBufPair);
    
    return geomListPair;
} 

pair<CollectiveAttrGeom*, CollectiveAttrGeom*>* BufferManagerForGeoms :: getAllToAllData()
{
   int numProcesses = args->numProcesses;
   
   CollectiveInfo *sendBuffer = (CollectiveInfo *)malloc(numProcesses *sizeof(CollectiveInfo));
   CollectiveInfo *recvBuffer = (CollectiveInfo *)malloc(numProcesses *sizeof(CollectiveInfo));
   
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
 
   for(int pid = 0; pid < numProcesses; pid++) {

	   int layer1VCntForP = 0;
       int layer2VCntForP = 0;
       
       vector<int> *cells = processToCellsMapping->at(pid);
           
       for(int cellId : *cells) {
          Cell *cell = shapeInACell->at(cellId);
          
          list<Geometry*> *layerAGeom = cell->getLayerAGeom();
          
          for(list<Geometry*>::iterator it = layerAGeom->begin() ; it != layerAGeom->end(); ++it) {
             Geometry *geom = *it;
             ExtraInfo *info = (ExtraInfo*)geom->getUserData();
             layer1VCntForP += info->vertexStr.size(); ;
             
          }
          
           list<Geometry*> *layerBGeom = cell->getLayerBGeom();
           
           for(list<Geometry*>::iterator it = layerBGeom->begin() ; it != layerBGeom->end(); ++it) {
             Geometry *geom = *it;
             ExtraInfo *info = (ExtraInfo*)geom->getUserData();
             layer2VCntForP += info->vertexStr.size(); ;
		   }
       }
       
       sendBuffer[pid].layer1BufLen = layer1VCntForP;
       sendBuffer[pid].layer2BufLen = layer2VCntForP;
   }
   
   SpatialTypes types;
   MPI_Datatype MPI_COLL_INFO = types.createCollectiveInfoType();
    
   MPI_Alltoall(sendBuffer, 1, MPI_COLL_INFO, recvBuffer, 1, MPI_COLL_INFO, MPI_COMM_WORLD);
   
   CollectiveAttrGeom *l1Attributes = new CollectiveAttrGeom(numProcesses);
   l1Attributes->populateL1CollectiveAttributes(sendBuffer, recvBuffer, numProcesses);
   
   CollectiveAttrGeom *l2Attributes = new CollectiveAttrGeom(numProcesses);
   l2Attributes->populateL2CollectiveAttributes(sendBuffer, recvBuffer, numProcesses);
  
   return new pair<CollectiveAttrGeom*, CollectiveAttrGeom*>(l1Attributes, l2Attributes);
}

//pair<map<int,vector<Geometry*>* > *, map<int,vector<Geometry*>* >* >*

pair<pair<char *,int>*, pair<char *,int>* >* BufferManagerForGeoms :: shuffleExchangeGeoms(
													pair<CollectiveAttrGeom*, CollectiveAttrGeom*>* attrPair)
{
  // for 1st layer
  CollectiveAttrGeom *l1Attr = attrPair->first;
  
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
  
  // for 2nd layer
  
  CollectiveAttrGeom *l2Attr = attrPair->second;
  char *sendL2Buffer = populateL2SendBuffer(l2Attr, grid, args->numProcesses);

  assert(l2Attr->recvBufSize >= 0 && l2Attr->recvBufSize <= INT_MAX);
  
  char *recvL2Buffer = (char *)malloc(sizeof(char) * l2Attr->recvBufSize); 
  
  if(recvL2Buffer == NULL) {
      cerr<<"Malloc Failed before layer 2 mpi_alltoallv l2Attr->recvBufSize "<<l2Attr->recvBufSize<<endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
      exit(0);
  }
  
  MPI_Alltoallv(sendL2Buffer, l2Attr->sendCountsArr, l2Attr->sdispls, MPI_CHAR, 
                recvL2Buffer, l2Attr->recvCountArr, l2Attr->rdispls, MPI_CHAR, MPI_COMM_WORLD);

  pair<char *,int> *pairL2 = new pair<char *,int>(recvL2Buffer, l2Attr->recvBufSize);

  
  return new pair<pair<char *,int>*, pair<char *,int>* >(pairL1, pairL2);
}


char* BufferManagerForGeoms :: populateL1SendBuffer(CollectiveAttrGeom *attr, Grid *grid, int numProcesses)
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

char* BufferManagerForGeoms :: populateL2SendBuffer(CollectiveAttrGeom *attr, Grid *grid, int numProcesses)
{
   int sendBufSize = attr->sendBufSize;

   assert(sendBufSize>=0 && sendBufSize <= INT_MAX);

   char *sendBuf = (char *)malloc(sendBufSize * sizeof(char));

   if(sendBuf == NULL) {
      cerr<<"Malloc Failed in populateL2SendBuffer sendBufSize "<<sendBufSize<<endl;
      MPI_Abort(MPI_COMM_WORLD, 1);
      exit(0);
   }

   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
   int buffCounter = 0;
   
   for(int pid = 0; pid < numProcesses; pid++) {
       
       vector<int> *cells = processToCellsMapping->at(pid);
           
       for(int cellId : *cells) {
          Cell *cell = shapeInACell->at(cellId);
          list<Geometry*> *geoms = cell->getLayerBGeom();  // this part is different from the other method
          
          packGeoms( geoms, sendBuf, &buffCounter);
          
          assert(buffCounter <= sendBufSize);
          
       }
   }
   //cout<<" # "<<envCounter<<" ";
   return sendBuf;
}


// serialization from envelope to struct bbox
void BufferManagerForGeoms :: packGeoms( list<Geometry*> *geoms, char *sendBuf, int *buffCounter)
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


/*
 This is required for MPI_AllToAllv communication 
   because a process does not know how many it will receive from other fellow processes

pair<CollectiveAttributes*, CollectiveAttributes*>* BufferManagerForGeoms :: getAllToAllData()
{
   int numProcesses = args->numProcesses;
   
   long *sendBuffer = (long *)malloc(2*numProcesses *sizeof(long));
   long *recvBuffer = (long *)malloc(2*numProcesses *sizeof(long));
   
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   geos::io::WKTWriter wktWriter;
   
   list<string> *polyStrL1 = new list<string>();
   list<string> *polyStrL2 = new list<string>();
   
   vector<GeomInfo> geomL1Info;
   
   long layer1VCntForP = 0;
   long layer2VCntForP = 0;
   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
   
   for(int pid = 0; pid < numProcesses; pid++) {
   	   
       vector<int> *cells = processToCellsMapping->at(pid);
         	  
       for(int cellId : *cells) {
          Cell *cell = shapeInACell->at(cellId);
          list<Geometry*> *layerAGeom = cell->getLayerAGeom();
          
          for(list<Geometry*>::iterator it = layerAGeom->begin() ; it != layerAGeom->end(); ++it) {
             Geometry *geom = *it;
             string str1 = wktWriter.write(geom);
             
             GeomInfo l1Info;
             //l1Info.length = str1.size();
             //l1Info.cellId = cellId;
             
             geomL1Info.push_back(l1Info);
// 				stringstream ss;
//              ss << cellId <<":";
//              string cellIdStr = ss.str();
//              str1 = str1 + cellIdStr;
             
             polyStrL1->push_back(str1);
             layer1VCntForP += str1.size();
             
          }
          
           list<Geometry*> *layerBGeom = cell->getLayerBGeom();
           
           for(list<Geometry*>::iterator it = layerBGeom->begin() ; it != layerBGeom->end(); ++it) {
             Geometry *geom = *it;
             string str2 = wktWriter.write(geom);
             
             // stringstream ss;
//              ss << cellId <<":";
//              string cellIdStr = ss.str();
//              str2 = str2 + cellIdStr;
           
             polyStrL2->push_back(str2);
             
             layer2VCntForP += str2.size();
          }
       
       }
       
       int pos = 2 * pid;
       sendBuffer[pos] = layer1VCntForP;
       sendBuffer[pos + 1] = layer2VCntForP;
   }
   
   //printf(" Debug Before all to all \n");
   
   MPI_Alltoall(sendBuffer, 2, MPI_LONG, recvBuffer, 2, MPI_LONG, MPI_COMM_WORLD);
   
   //printf(" Debug After all to all \n");
   
   //CollectiveAttributes *l1Attributes = populateL1CollectiveAttributes(sendBuffer, recvBuffer, numProcesses);
   //CollectiveAttributes *l2Attributes = populateL2CollectiveAttributes(sendBuffer, recvBuffer, numProcesses);
   

   return nullptr;//new pair<CollectiveAttributes*, CollectiveAttributes*>(l1Attributes, l2Attributes) ;
}
*/

