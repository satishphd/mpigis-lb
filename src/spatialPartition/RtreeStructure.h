#ifndef RTREE_STRUCTURE_H
#define RTREE_STRUCTURE_H

#include<unordered_map>
#include <geos/geom/Geometry.h>
#include <geos/geom/CoordinateSequence.h>

#include "cell.h"
//#include "rTreeIndex.h"

//class Geometry;

using namespace std;

struct KeyInfo
{
  unordered_map<int, Cell*> *shapesInCell;
  Geometry *geom;
  bool layer1;
};

class RtreeStructure
{
   //struct Node* rTreeRoot;
    
   public:

   RtreeStructure()
   {
     //rTreeRoot = RTreeNewIndex();
   }
   
   static int searchCallback(int cellId, void* key);
   
   void createRtreeFromCellMBRs( unordered_map<int, Envelope>* gridCells);
   
   int populateWithShapes(list<Geometry*> *shapes, unordered_map<int, Cell*> *shapesInCell, bool isBaseLayer);
   
};

#endif
