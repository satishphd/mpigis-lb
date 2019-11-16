#ifndef __UNIFORM_GRID_H_INCLUDED__
#define __UNIFORM_GRID_H_INCLUDED__

#include "grid.h"
#include "../index/index.h"
//#include "RtreeStructure.h"

#include<cstdlib>
//#include <geos/geom/Coordinate.h>
#include <tuple>
#include <mpi.h>

//class geos::geom::CoordinateSequence;

class UniformGrid : public Grid
{
 Index tree;
 int partition();
 
 public:

 UniformGrid(int cells, Envelope *universe) : Grid(cells, universe)
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
 
// int populateGridCells(list<Envelope*>* envs, bool isBaseLayer);

 list<string> localGridStatistics();
 
 map<int, pair<int, int> > globalGridStatistics(int *gridShapeCounts);
 
 void printGlobalGridStatistics( map<int, pair<int, int> > &map);
 
 private:

 int partitionHelper(int numberOfPartitions);



void addToMBR(int index, double lowX, double lowY, double upX,   double upY);     

void partitionInto2cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto4Cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto8cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto16Cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto32Cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto64Cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto128Cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto256Cells(int index, double lowX, double lowY, double upX, double upY);

void partitionInto512Cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto1024Cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto2048Cells(int index,double lowX, double lowY, double upX, 
double upY);

void partitionInto4096Cells(int index, double, double, double, double);

void partitionInto8000Cells(int index, double, double, double, double);

void partitionInto16kCells(int index, double, double, double, double);

void partitionInto32kCells(int index, double, double, double, double);

void partitionInto64kCells(int index, double, double, double, double);

void partitionInto128kCells(int index, double, double, double, double);

void partitionInto256kCells(int index, double, double, double, double);


};

#endif 