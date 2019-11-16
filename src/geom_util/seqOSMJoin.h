#ifndef MYGEOS_H
#define MYGEOS_H

#include <geos/geom/prep/PreparedGeometryFactory.h>
#include <geos/geom/prep/PreparedGeometry.h>
#include <geos/geom/GeometryFactory.h>


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
//#include <geos/geom/MultiGeometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/PrecisionModel.h>

#include <geos/util/IllegalArgumentException.h>
// std
#include <vector>
#include <cstring> // std::size_t

#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/geom/Geometry.h>
//#include <geos/geom/Coordinate.h>
#include <fstream>
#include <stdlib.h>
#include <sys/time.h>
//#include "shapefil.h"
#include <geos/index/strtree/STRtree.h>
#include "../parser/WKTParser.h"
#include "../index/index.h"
#include "../join/join.h"
#include "../filePartition/shapefil.h"

using namespace std;
using namespace geos::geom;
using geos::geom::Coordinate;



class MyGeos
{
  geos::index::strtree::STRtree index;

  void distRead(int numRecords, SHPHandle hSHP, SHPObject ***psShape);
  void destoryObjects(SHPObject **psShape,int num);
  void populateLayer(SHPObject **psShape,int num, vector<Geometry*> *layer);

  public:
  
  void testLoadBalance(list<Envelope*>* mbrs, vector<Geometry*> *layerA);
  
  void printMBRs(list<Envelope*>* mbrs);
  //void distRead(int numRecords, SHPHandle hSHP, SHPObject ***psShape);
  //void destoryObjects(SHPObject **psShape,int num);
  //void populateLayer(SHPObject **psShape,int num, vector<Geometry*> *layer);

  //vector<Geometry*>* readShapefile(char* filename);
  vector<Geometry*>* readOSM(char* filename);
  vector<Geometry*>* readCSVFile(char* filename); 
  vector<Geometry*>* readShapefile(char* filename);
  
  void query(vector<Geometry*> *poly);
  void createIndex(vector<Geometry*> *poly); 
  static double my_difftime ();  
};

#endif