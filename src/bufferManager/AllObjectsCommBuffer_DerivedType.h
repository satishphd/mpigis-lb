#ifndef ALLOBJECTS_BUFFERMANAGER_GEOMS_H
#define ALLOBJECTS_BUFFERMANAGER_GEOMS_H


#include "MultiRoundOneLayerBufferMBR.h"
#include "allobjectsCollectiveAttr.h"
#include<math.h>

/*#include "../filePartition/config.h"
#include "../spatialPartition/grid.h"
#include "../taskMap/TaskMap.h"
#include "allobjectsCollectiveAttr.h"
#include "../mpiTypes/mpitype.h"
#include "../parser/WKTParser.h"

#include "../parser/road_network_parser.h"

#include <climits>
#include<assert.h>
#include<list>
#include<map>
#include<tuple>
#include<geos/geom/Geometry.h>
*/

#define assert_msg(x) !(std::cerr << "Assertion failed: " << x << std::endl)

class AllObjectsCommBuffer 
{
  int numRounds = 10;
  
  const int PACKING_LEN = 10000;
  
  Grid *grid;
  
  MappingStrategy *strategy;
  
  Config *args;
  
  AllObjectsCollectiveAttr* m_coll_attr;
  
  MPI_Datatype create_MPI_CHARS_Type();
  
  long countGeoms( map<int, list<Geometry*>* >* geomListPair);
  
  void packGeoms( list<Geometry*> *geoms, char *sendBuf, long *buffCounter);
  
  char* populateL1SendBuffer(AllObjectsCollectiveAttr *attr, Grid *grid, int numProcesses,
  								int currRound, MultiRoundOneLayerBufferMBR *mbrMgr);
  
  char* populateL2SendBuffer(AllObjectsCollectiveAttr *attr, Grid *grid, int numProcesses);
  
  AllObjectsCollectiveAttr* getAllToAllData(int currRound, MultiRoundOneLayerBufferMBR *mbrMgr);
  
  //pair< pair< bbox*, int>* , pair< bbox*, int>* >* getControlData(MappingStrategy *strategy, Grid *grid, Config *args);
  
  //pair<map<int,vector<Geometry*>* > *, map<int,vector<Geometry*>* >* >* 
  
  pair<char *,long>* shuffleExchangeGeoms(AllObjectsCollectiveAttr* attrPair, int currRound, 
  											MultiRoundOneLayerBufferMBR *mbrMgr, MPI_Datatype MPI_CHARs_NEW);  
  
  // void addToGlobalResultMap( map<int, list<Geometry*>* >* localMap, 
// 												map<int, list<Geometry*>* >*globalMap);
  
  public:
  AllObjectsCommBuffer(MappingStrategy *strategy, Grid *grid, Config *args)
  {
       this->grid = grid;
       this->strategy =  strategy;
       this->args = args;
       
//       m_GeomListPair = initCommunication(strategy, grid, args);
  }
  
  // AllObjectsCollectiveAttr* getCollectiveAttributes()
//   {
//       return m_coll_attr;
//   } 
  
  void setCollectiveAttributes( AllObjectsCollectiveAttr* attr) {
    m_coll_attr = attr;
  }
  
  list<Geometry*>* shuffleExchange();

  map<int, list<Geometry*>* >* shuffleExchangeGrpByCell();
};

#endif