#ifndef __OVERLAY_H_INCLUDED__
#define __OVERLAY_H_INCLUDED__

#include<geos/geom/Geometry.h>
#include <geos/index/strtree/STRtree.h>
#include <geos/geom/prep/PreparedGeometryFactory.h>
#include <geos/geom/prep/PreparedGeometry.h>
#include<list>
#include<map>
#include<tuple>
#include<string>
#include <fstream>

using namespace geos::geom;
using namespace std;

class Overlay
{
  ofstream wktfile;
  
  void write(Geometry *geom);

  public:

  list<Geometry*>* overlay(map<Geometry*, vector<void *> >* intersectionMap);
  bool initOutputFile(int pid);
  void writeGeoms(list<Geometry*> *geom);
  void closeFile();
};

#endif 