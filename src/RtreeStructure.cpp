#include "spatialPartition/RtreeStructure.h"

void RtreeStructure :: createRtreeFromCellMBRs( unordered_map<int, Rect>* gridCells)
{	
	// Build R-tree from grid cells
	for(auto &elem : *gridCells)
	{	
	  // i+1 is rect ID because 0 has special meaning.
	  // Note that r-tree root can change
	  RTreeInsertRect(&elem.second, elem.first, &rTreeRoot, 0);
	}
}

// key contains the info of the searched polygon
 int RtreeStructure :: searchCallback(int cellId, void* polyKey) 
{
   cout<<"HI ";
   
   /* Note: -1 to make up for the +1 when data was inserted */
   cellId = cellId - 1; 
   
   KeyInfo *key = (KeyInfo *)polyKey;
   
   Geometry *geom = key->geom;
   bool isBase = key->layer1;

   unordered_map<int, Cell*> *cellMap = key->shapesInCell;  
    
   Cell *cell = cellMap->at(cellId);

   if(isBase)
     cell->addToLayerA(geom);
   else
     cell->addToLayerB(geom);
     
   return 0;
 }


// struct KeyInfo
// {
//   unordered_map<int, Cell*> *shapesInCell;
//   Geometry *geom;
//   bool layer1;
// };

int RtreeStructure :: populateWithShapes(list<Geometry*> *shapes, unordered_map<int, Cell*> *shapesInCell, 
                       bool isBaseLayer)
{
  cout<<"RtreeStructure populateWithShapes: shapes "<<shapes->size()<<endl;
  
  for(auto &shape : *shapes)
  {
     //int numGeoms = shape->getNumGeometries();
     
     //for(int i = 0; i< numGeoms; i++)
     //{
        //const Geometry *geom = shape->getGeometryN(i);

        Geometry *mbr = shape->getEnvelope();
        geos::geom::CoordinateSequence *coords = mbr->getCoordinates();
     
      	//int cords = seq->size();
        //cout<< "Size of seq "<<cords;

        Rect rect = {{coords->getX(0), coords->getY(0), coords->getX(1), coords->getY(1)}};

        KeyInfo polygonKey;
        
        polygonKey.geom = shape;
        polygonKey.layer1 = isBaseLayer;
        polygonKey.shapesInCell = shapesInCell;
        
        RTreeSearch(rTreeRoot, &rect, searchCallback, &polygonKey);
     //}
  }
  
  return 0;
}
