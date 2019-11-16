#ifndef MULTI_ROUND_BUFFERMANAGER_GEOMS_H
#define MULTI_ROUND_BUFFERMANAGER_GEOMS_H

#include "MultiRoundOneLayerBufferMBR.h"
#include <climits>

class MultiRoundOneLayerGeomBufferMgr 
{
  int numRounds = 10;
  
  Grid *grid;
  
  MappingStrategy *strategy;
  
  Config *args;
  
  CollectiveAttrGeom* m_coll_attr;
  
  long countGeoms( map<int, list<Geometry*>* >* geomListPair);
  
  void packGeoms( list<Geometry*> *geoms, char *sendBuf, int *buffCounter);
  
  char* populateL1SendBuffer(CollectiveAttrGeom *attr, Grid *grid, int numProcesses,
  								int currRound, MultiRoundOneLayerBufferMBR *mbrMgr);
  
  char* populateL2SendBuffer(CollectiveAttrGeom *attr, Grid *grid, int numProcesses);
  
  CollectiveAttrGeom* getAllToAllData(int currRound, MultiRoundOneLayerBufferMBR *mbrMgr);
  
  //pair< pair< bbox*, int>* , pair< bbox*, int>* >* getControlData(MappingStrategy *strategy, Grid *grid, Config *args);
  
  //pair<map<int,vector<Geometry*>* > *, map<int,vector<Geometry*>* >* >* 
  
  pair<char *,int>* shuffleExchangeGeoms(CollectiveAttrGeom* attrPair, int currRound, 
  											MultiRoundOneLayerBufferMBR *mbrMgr);  
  
  void addToGlobalResultMap( map<int, list<Geometry*>* >* localMap, 
												map<int, list<Geometry*>* >*globalMap);
  
  public:
  MultiRoundOneLayerGeomBufferMgr(MappingStrategy *strategy, Grid *grid, Config *args)
  {
       this->grid = grid;
       this->strategy =  strategy;
       this->args = args;
       
//       m_GeomListPair = initCommunication(strategy, grid, args);
  }
  
  CollectiveAttrGeom* getCollectiveAttributes()
  {
      return m_coll_attr;
  } 
  
  void setCollectiveAttributes( CollectiveAttrGeom* attr) {
    m_coll_attr = attr;
  }
  
  list<Geometry*>* shuffleExchange();

  map<int, list<Geometry*>* >* shuffleExchangeGrpByCell();
};

#endif