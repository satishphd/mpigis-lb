#ifndef ADAPTIVE_GRID_H
#define ADAPTIVE_GRID_H


#include "grid.h"
#include "../index/index.h"
//#include "RtreeStructure.h"

#include<cstdlib>
//#include <geos/geom/Coordinate.h>
#include <tuple>
#include <mpi.h>
#include "../geom_util/fileRead.h"

class AdaptiveGrid : public Grid
{
 Index tree;
 int partition();
 
 public:

 AdaptiveGrid(int cells, Envelope *universe) : Grid(cells, universe)
 {
   gridCells = new map<int, Envelope*>(); // represents cellId -> cell MBR 
   
   partition();
  
   tree.createRTree(gridCells);
   
   initShapesInCell();
 }
 
 void initShapesInCell();
 
 void printGridCoordinates();
 
 int* numShapesPerCell(); 
 
 //int populateWithShapes(list<Geometry*>);
 int populateGridCells(list<Geometry*>* shapes, bool isBaseLayer);

 map<int, list<Geometry*>* >* gridPartition(list<Geometry*>* shapes);
 
 list<string> localGridStatistics();
 
 map<int, pair<int, int> > globalGridStatistics(int *gridShapeCounts);
 
 void printGlobalGridStatistics( map<int, pair<int, int> > &map);
};

#endif


