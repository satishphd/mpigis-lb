#ifndef __INDEX_H_INCLUDED__
#define __INDEX_H_INCLUDED__

#include<geos/geom/Geometry.h>
#include <geos/index/strtree/STRtree.h>
#include <geos/index/quadtree/Quadtree.h>

#include<list>
#include<map>
#include<unordered_map>
#include "../spatialPartition/cell.h"
#include "../mpiTypes/mpitype.h"

using namespace geos::geom;
//using namespace geos::index::strtree;
//using namespace geos::index::quadtree;

using namespace std;

class Index
{
  //geos::index::strtree::STRtree index;
  geos::index::SpatialIndex *index;
  void createIndex(list<Geometry*> *poly); 
  void createIndex(vector<Geometry*> *poly); 
  
  public:
  
  void createRTree(vector<GeomInfo*> *envs);

  void createRTree(vector<Envelope*> *envs);
  
  void createRTree(list<Envelope*> *envs);
  
  void createRTree(list<Geometry*> *poly);
  
  void createRTree(vector<Geometry*> *poly);
  
  void createRTree(map<int, Envelope*> *cellMap);
  
  void createQuadTree(list<Geometry*> *poly);
  
  map<Geometry*, vector<void *> >* query(list<Geometry*> *poly);
  
  map<Geometry*, vector<void *> >* query(vector<Geometry*> *poly);
  
  map<Envelope*, vector<void *> >* query(vector<Envelope*> *envs);
    
  map<Envelope*, vector<void *> >* query(list<Envelope*> *envs);
  
  map<Envelope*, vector<void *> >* query(vector<GeomInfo*> *envs);
  
  int populateWithShapes(list<Geometry*> *shapes, map<int, Cell*> *shapesInCell, bool isBaseLayer);
  
//  int populateWithShapes(list<Envelope*> *shapes, map<int, Cell*> *shapesInCell, bool isBaseLayer);
  
  map<int, list<Geometry*>* >* partitionGeomsAmongGridCells(int aNumCells, list<Geometry*> *shapes);

};

#endif 