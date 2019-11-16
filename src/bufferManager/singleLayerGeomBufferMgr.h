#ifndef SINGLELAYER_BUFFERMANAGER_GEOMS_H
#define SINGLELAYER_BUFFERMANAGER_GEOMS_H

#include "singleLayerMBRBufferMgr.h"
#include <climits>

class SingleLayerGeomBufferMgr 
{
  Grid *grid;
  
  MappingStrategy *strategy;
  
  Config *args;
  
  CollectiveAttrGeom* m_coll_attr;
  
  void packGeoms( list<Geometry*> *geoms, char *sendBuf, int *buffCounter);
  
  char* populateL1SendBuffer(CollectiveAttrGeom *attr, Grid *grid, int numProcesses);
  
  char* populateL2SendBuffer(CollectiveAttrGeom *attr, Grid *grid, int numProcesses);
  
  CollectiveAttrGeom* getAllToAllData();
  
  //pair< pair< bbox*, int>* , pair< bbox*, int>* >* getControlData(MappingStrategy *strategy, Grid *grid, Config *args);
  
  //pair<map<int,vector<Geometry*>* > *, map<int,vector<Geometry*>* >* >* 
  
  pair<char *,int>* shuffleExchangeGeoms(CollectiveAttrGeom* attrPair);  
  
  int countGeoms( map<int, list<Geometry*>* >* geomListPair);
  
  public:
  SingleLayerGeomBufferMgr(MappingStrategy *strategy, Grid *grid, Config *args)
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