#include "taskMap/roundRobinTaskMap.h"

int RoundRobinStrategy :: initProcessToCellsMap()
{
  for(int pid = 0; pid<numProcesses; pid++) {
    
    vector<int> *cellsForAProcess = new vector<int>();
    processToCellsMap->insert({pid, cellsForAProcess});
  }
  
  return 0;
}

int RoundRobinStrategy :: createProcessToCellsMap()
 {
    /* Now create processToCellsMap: map of process id to list of cell_ids from the previous cellToProcessMap map */
    
    for(auto it = cellToProcessMap->begin(); it != cellToProcessMap->end(); ++it)
    {
        int cellId = it->first;
        int pid = it->second;
        
        vector<int> *cells = processToCellsMap->at(pid);
        cells->push_back(cellId);
    }
    return 0;
 }

 int RoundRobinStrategy :: createCellProcessMap( map<int, Envelope*> *grid)
 {
    // cell to process mapping : you have to know which cells you have to handle
    // this is how a process will assert its ownership on the the cell(s)
    
    for(auto it = grid->begin(); it != grid->end(); ++it)
    {
        int cellId = it->first;
        cellToProcessMap->insert({cellId, cellId % numProcesses});
    }
    
    createProcessToCellsMap();
    
    return 0;
 }
 
 /* use by one process */
 void RoundRobinStrategy :: printStrategy()
 {
    //map<int, vector<int>* >* processToCellsMap;  // process id to cell id
    for(auto it = processToCellsMap->begin(); it != processToCellsMap->end(); ++it)
    {
       
       vector<int> *cellPerProcess = it->second;
       cout<<"Process "<< it->first<<":"<<cellPerProcess->size();
       // for(auto cell : *cellPerProcess) {
//             cout<<cell<<"  ";
//        }
       cout<<endl;
    }
 }