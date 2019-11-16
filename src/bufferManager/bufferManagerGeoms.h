#ifndef BUFFERMANAGER_GEOMS_H
#define BUFFERMANAGER_GEOMS_H


#include "bufferManagerMBR.h"
#include <climits>

class BufferManagerForGeoms 
{
  Grid *grid;
  
  MappingStrategy *strategy;
  
  Config *args;
  
  void packGeoms( list<Geometry*> *geoms, char *sendBuf, int *buffCounter);
  
  char* populateL1SendBuffer(CollectiveAttrGeom *attr, Grid *grid, int numProcesses);
  
  char* populateL2SendBuffer(CollectiveAttrGeom *attr, Grid *grid, int numProcesses);
  
  pair<CollectiveAttrGeom*, CollectiveAttrGeom*>* getAllToAllData();
  
  //pair< pair< bbox*, int>* , pair< bbox*, int>* >* getControlData(MappingStrategy *strategy, Grid *grid, Config *args);
  
  //pair<map<int,vector<Geometry*>* > *, map<int,vector<Geometry*>* >* >* 
  
  pair<pair<char *,int>*, pair<char *,int>* >*shuffleExchangeGeoms(pair<CollectiveAttrGeom*, 
  																	CollectiveAttrGeom*>* attrPair);  
  public:
  BufferManagerForGeoms(MappingStrategy *strategy, Grid *grid, Config *args)
  {
       this->grid = grid;
       this->strategy =  strategy;
       this->args = args;
       
//       m_GeomListPair = initCommunication(strategy, grid, args);
  }

  pair<list<Geometry*>*, list<Geometry*>* >* shuffleExchange();

  pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >* shuffleExchangeGrpByCell();
};

#endif