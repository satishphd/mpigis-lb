#ifndef __UTIL_H_INCLUDED__
#define __UTIL_H_INCLUDED__

//#include<geos.h>
#include<geos/geom/Geometry.h>
#include<geos/geom/Polygon.h>
#include<list>
#include<map>
#include<vector>
#include<mpi.h>

using namespace geos::geom;
using namespace std;
//using namespace geos::operation;

class GeometryUtility 
{
  public:
  // Envelope is x1, x2, y1, y2
  static Envelope getMBR(list<Geometry*> *geoms);
  static Geometry* unionGeoms(list<Geometry*> *geoms);
  static int estimateLoad(map<Geometry*, vector<void *> >*joinResult);
  
  
  private:
  GeometryUtility() {}
  
};

#endif 

