#ifndef __POINT_NODE_PARSER_H_INCLUDED__
#define __POINT_NODE_PARSER_H_INCLUDED__

//#include <geos/geom/Geometry.h>
#include "Parser.h"
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Point.h>
#include <sstream>
//#include <geos/geom/CoordinateArraySequence.h>
//#include <geos/geom/CoordinateSequenceFactory.h>

struct pointNode
{
  string tag;
  long nodeId;
  
  pointNode(string &aTag, long aNodeId)
  {
     tag = aTag;
     nodeId = aNodeId;
  }
};

class PointNodeParser
{
  geos::geom::GeometryFactory factory;
  
  Geometry* parseString( const pair<int, string*> &p);
  //pair<int, string*> extract(const string &string);
  
  public:
  Geometry* extract(const string &str);
   
  list<Geometry*>* parse(const FileSplits &split);
  //list<Geometry*>* readOSM(char* filename);
  ~PointNodeParser() {}
   
};

#endif