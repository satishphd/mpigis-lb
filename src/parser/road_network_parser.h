#ifndef __ROAD_NETWORK_PARSER_H_INCLUDED__
#define __ROAD_NETWORK_PARSER_H_INCLUDED__

#include "Parser.h"
//#include "../filePartition/FileSplits.h"

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/LineString.h>

#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h> 

struct RoadNode
{
  public:
  
  long edgeId;
  long startNodeId;
  long endNodeId;
  long wayId;
  
  string tags;
  

  RoadNode(long aEdgeId, long aStart, long aEnd, long aWayId, string aTags) 
  {
  	edgeId = aEdgeId;
  	startNodeId = aStart;
  	endNodeId = aEnd;
  	wayId  = aWayId;
  	tags   = aTags;
  }
};

class RoadNetworkParser
{
  Geometry* parseString( const pair<int, string*> &p);
  //pair<int, string*> extract(const string &string);
  
  public:
  Geometry* extract(const string &str);
   
  list<Geometry*>* parse(const FileSplits &split);
  //list<Geometry*>* readOSM(char* filename);
  ~RoadNetworkParser() {}
   
};


#endif