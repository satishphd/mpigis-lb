#ifndef FILTER_H
#define FILTER_H

#include<iostream>
#include <fstream>
#include <stdlib.h>
#include<stdio.h>
#include<vector>
#include<list>

//#include<geos.h>
#include <geos/geom/Geometry.h>
#include<geos/geom/CoordinateSequence.h>
#include<geos/geom/Coordinate.h>

using namespace std;
using namespace geos::geom;

class Tile
{
  public:
  
  int startIndex;
  int endIndex;
  Envelope tileEnv;
  
 
  Tile(int start, int end)
  {
	  startIndex = start;
	  endIndex = end;
  }
  
  void setRect(Envelope r)
  {
	  tileEnv = r;
  }
};

class Filter
{
   public:
   const static int NUM_TILES = 10;
   
   int test(list<Tile*>* xTiles, Geometry* y);
   
   /* This method is to create a leaf node of the R-tree for a subset of vertices in a polygon */
   Tile* createTile(Geometry* x, int first, int last);
   
   list<Tile*>* getTiles(Geometry *g);
};

#endif
