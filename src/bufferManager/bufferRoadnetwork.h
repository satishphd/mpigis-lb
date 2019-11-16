#ifndef __ROAD_NETWORK_BUFFER_H_INCLUDED__
#define __ROAD_NETWORK_BUFFER_H_INCLUDED__

//#include "bufferManagerMBR.h"
#include "../filePartition/config.h"
#include "../spatialPartition/grid.h"
#include "../taskMap/TaskMap.h"
#include "collectiveAttributes.h"
#include "../mpiTypes/mpitype.h"
#include "../parser/WKTParser.h"

#include "../parser/road_network_parser.h"

#include <climits>
#include<assert.h>
#include<vector>
#include<map>
#include<tuple>
#include<geos/geom/Geometry.h>

using namespace std;
using namespace geos::geom;	

class BufferRoadNetwork
{
  private:
  CollectiveAttributes* m_attr;
  
  Grid *grid;
  MappingStrategy *strategy;
  Config *args;
  
  CollectiveAttributes* getAllToAllData();
  
  int calcBufferSize(int *sendBuffer, int nprocs);
  CollectiveAttributes* populateCollectiveAttributes(int *sendBuffer, int *recvBuffer, int nprocs);
  
  int* prefixSum(int *orig, int length);                                         

  int *popRecvArgs(int *recvShpCntBuffer, int nprocs, int *aTotalReceiveBaseBoxes);
  
  roadbox* populateSendBuffer(CollectiveAttributes *attr, Grid *grid, int numProcesses);
  
  void packEnvelope(int cellId, list<Geometry*> *geoms, roadbox *envelopes, int *envCounter);
    
  pair<roadbox*, int>* exchangeEnvelopes(roadbox *envelopes, CollectiveAttributes *attr);
  
  void init();
    
  map<int, list<RoadInfo*>* >* parseEnvelopesFromStruct(pair<roadbox*, int>* recvBoxes);
  
  public:
  BufferRoadNetwork(MappingStrategy *strategy, Grid *grid, Config *args)
  {
       this->grid = grid;
       this->strategy =  strategy;
       this->args = args;
       
       init();     
  }
  
  CollectiveAttributes* getCollectiveAttributes()
  {
      return m_attr;
  } 
  
  map<int,list<RoadInfo*>* >* shuffleExchangeByCell();
};

#endif