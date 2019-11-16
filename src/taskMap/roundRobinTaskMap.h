#ifndef ROUNDROBINTASKMAP_H
#define ROUNDROBINTASKMAP_H


#include "TaskMap.h"

class RoundRobinStrategy: public MappingStrategy
{
  int initProcessToCellsMap();
  
  int createProcessToCellsMap();
  
  public:
  
  RoundRobinStrategy(int cells, int processes)
  {
    this->numCells = cells;
    this->numProcesses = processes;
    
    processToCellsMap = new map<int, vector<int>* >();
    initProcessToCellsMap();
    
    cellToProcessMap = new map<int, int>();
  }
  
  int createCellProcessMap( map<int, Envelope*> *grid);
  
  void printStrategy();
  
};

#endif
 