#ifndef __SINGLE_LAYER_MBRBUFFER_H_INCLUDED__
#define __SINGLE_LAYER_MBRBUFFER_H_INCLUDED__


#include "../filePartition/config.h"
#include "../spatialPartition/grid.h"
#include "../taskMap/TaskMap.h"
#include "collectiveAttributes.h"
#include "../mpiTypes/mpitype.h"
#include "../parser/WKTParser.h"

#include "../parser/road_network_parser.h"

#include <climits>
#include<assert.h>
#include<list>
#include<map>
#include<tuple>
#include<geos/geom/Geometry.h>

using namespace std;
using namespace geos::geom;	

class SingleLayerMBRBuffer
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

  
  
  bbox* populateSendBuffer(CollectiveAttributes *attr, Grid *grid, int numProcesses);
  
  void packEnvelope(int cellId, list<Geometry*> *geoms, bbox *envelopes, int *envCounter);
    
  pair<bbox*, int>* exchangeEnvelopes(bbox *envelopes, CollectiveAttributes *attr);
  
  void init();
    
  map<int, list<GeomInfo*>* >* parseEnvelopesFromStruct(pair<bbox*, int>* recvBoxes);
  
  public:
  SingleLayerMBRBuffer(MappingStrategy *strategy, Grid *grid, Config *args)
  {
       this->grid = grid;
       this->strategy =  strategy;
       this->args = args;
       
       init();     
  }
  
  map<int,list<GeomInfo*>* >* shuffleExchangeByCell();

  pair< bbox*, int>* shuffleExchangeMBR();
  
  void printCollectiveAttributes(CollectiveAttributes *attr, int nprocs);
};

#endif