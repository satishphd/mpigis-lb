#ifndef __GRID_H_INCLUDED__
#define __GRID_H_INCLUDED__

#include <map>
//#include "rTreeIndex.h"
#include <list>
//#include <geos/geom/Geometry.h>
#include "cell.h"

using namespace std;
using namespace geos::geom;							 

  class Grid
  {
	protected:
	int numCells;
	
	Envelope *universe;
	map<int, Envelope*> *gridCells;		  // cellid -> cellDimension
	map<int, Cell*> *shapesInCell;  // cellid -> shapes it contains
	
	virtual int partition() = 0;
	
	public:
	
	Grid(int n, Envelope *universe)
	{
	  numCells = n;
	  this->universe = universe;
	}
	
	/*
	void setDimension(double xmin, double ymin, double xmax, double ymax)
	{
	  universe.boundary[0] = xmin;
	  universe.boundary[1] = ymin;
	  universe.boundary[2] = xmax;
	  universe.boundary[3] = ymax; 
	}*/
	
	map<int, Cell*> *getShapesInCell()
	{
	   return shapesInCell;
	}
	
	map<int, Envelope*> *getGridCellsMap() 
	{
	   return gridCells;
	}
	
	virtual int populateGridCells(list<Geometry*>*, bool isBaseLayer) = 0;
	
	//virtual int populateGridCells(list<Envelope*>*, bool isBaseLayer) = 0;
	
	virtual int* numShapesPerCell() = 0;
	 
	virtual void printGridCoordinates() = 0;
	virtual list<string> localGridStatistics() = 0;
	virtual map<int, pair<int, int> > globalGridStatistics(int *gridShapeCounts) = 0;
	virtual void printGlobalGridStatistics(map<int, pair<int, int> > &cellMap) = 0;
  };

#endif 