#ifndef CONTROL_DATA_MANAGER_H
#define CONTROL_DATA_MANAGER_H

// #include<tuple>
// #include<geos/geom/Geometry.h>
// #include "../mpiTypes/mpitype.h"

#include "bufferManagerMBR.h"

#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>

using namespace std;
using namespace geos::geom;

class ControlDataMgr
{
   
  
   pair<GeomInfo*, int>* exchangeControlData(GeomInfo *send_GeomInfo, CollectiveAttributes *attr); 

   public:
   pair<vector<GeomInfo>*, vector<GeomInfo>* >* getAllToAllControlData(MappingStrategy *strategy, 
   																		 Grid *grid, Config *args);
   
   pair<vector<GeomInfo>*, vector<GeomInfo>* >* shuffleExchangeCntrlData(
										pair<vector<GeomInfo>*, vector<GeomInfo>* >* geomInfo,
   										pair<CollectiveAttributes*, CollectiveAttributes*>* collectiveAttr);
};

#endif
