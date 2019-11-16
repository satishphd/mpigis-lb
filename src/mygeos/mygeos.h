#ifndef __MYGEOS_H_INCLUDED__
#define __MYGEOS_H_INCLUDED__


//#include "utility.h"
//#include "tut/tut.hpp"
// geos
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/CoordinateArraySequenceFactory.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/Dimension.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiPoint.h>
#include <geos/geom/MultiLineString.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/PrecisionModel.h>

#include <geos/util/IllegalArgumentException.h>
// std
#include <vector>
#include <cstring> // std::size_t

#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/geom/Geometry.h>
//#include <geos/geom/Coordinate.h>

#include <stdlib.h>

#include "../filePartition/shapefil.h"
//#include "rTreeIndex.h"
#include <geos/index/strtree/STRtree.h>

using namespace std;
using namespace geos::geom;
using geos::geom::Coordinate;


class MyGeos
{
  geos::index::strtree::STRtree index;

  public:
 
  void distRead(int startIndex,int endIndex, SHPHandle hSHP, SHPObject ***psShape);
  void destoryObjects(SHPObject **psShape,int num);
  void populateLayer(SHPObject **psShape,int num, vector<Polygon*> *layer);
  vector<Polygon*>* readShapefile(char* filename);

  void join(list<Polygon*> *poly);

  list<Geometry*>* overlay(list<Geometry*> *poly);

  void createIndex(list<Geometry*> *poly);
  
  list<Geometry*>* process_overlay(list<Geometry*>* layerA, list<Geometry*>* layerB); 
};
#endif 