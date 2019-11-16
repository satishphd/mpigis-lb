#ifndef TASKMAP_H
#define TASKMAP_H

#include<map>
#include<vector>
#include <geos/geom/Geometry.h>

using namespace std;
using namespace geos::geom;

class MappingStrategy
{
  protected:
  int numCells;
  int numProcesses;
  
  map<int, vector<int>* >* processToCellsMap;  // process id to cell id
  map<int, int>* cellToProcessMap;
  
  //virtual int createProcessToCellsMap() = 0;
  
  public:
  map<int, vector<int>* >* getProcessToCellsMap() 
  {
    return processToCellsMap;
  }
  
  virtual int createCellProcessMap( map<int, Envelope*> *grid) = 0;
  virtual void printStrategy() = 0;
};

#endif