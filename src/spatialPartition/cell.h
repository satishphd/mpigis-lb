#ifndef CELL_H
#define CELL_H

#include <geos/geom/Geometry.h>
#include <list>

using namespace std;
using namespace geos::geom;

class Cell
{
  int owner;  // owner is a process
  list<Geometry*> *layerAGeom;
  list<Geometry*> *layerBGeom;
  
  public:
  Cell()
  {
    layerAGeom = new list<Geometry*>();
    layerBGeom = new list<Geometry*>();
  }
  
  void addToLayerA(Geometry* geom)
  {
    layerAGeom->push_back(geom);
  }
  
  void addToLayerB(Geometry* geom)
  {
    layerBGeom->push_back(geom);
  }
  
  list<Geometry*> *getLayerAGeom() 
  {
     return layerAGeom;
  }
  
  list<Geometry*> *getLayerBGeom() 
  {
     return layerBGeom;
  }
};

#endif