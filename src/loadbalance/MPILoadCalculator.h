#ifndef __MPILOADCALCULATOR_H_INCLUDED__
#define __MPILOADCALCULATOR_H_INCLUDED__

#include <string>
#include <list>

#include "../filePartition/FileSplits.h"
#include "../parser/Parser.h"
#include "../parser/WKTParser.h"
#include "../filePartition/config.h"

#include "../index/index.h"
#include "../join/join.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <geos/geom/Geometry.h>

#include<mpi.h>
//#include "loadbalance/MPILoad.h"

#include "../geom_util/fileRead.h"

#include <fstream>
#include <unistd.h>

#define WORKTAG     1
#define DIETAG     2


typedef struct
{
   long pairs;
   int fileID; // file is a cell
}FilterResult;

using namespace std;

class MPILoadCalculator
{
   public:
   MPI_Datatype FilterResType;
    
   MPILoadCalculator()
   {
     FilterResType = createStructType();
   }
   
   MPI_Datatype createStructType() 
   {
    const int nitems=2;
    int blocklengths[2] = {1,1};
    MPI_Datatype types[2] = { MPI_LONG, MPI_INT};
    MPI_Datatype FILTER_RESULT; // = new MPI_Datatype();
    MPI_Aint     offsets[2];
   
    offsets[0] = offsetof(FilterResult, pairs);
    offsets[1] = offsetof(FilterResult, fileID);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &FILTER_RESULT);
    MPI_Type_commit(&FILTER_RESULT);
    return FILTER_RESULT;
   }
   
   virtual map<long, int, std::greater<long> >* calculateLoad( int numFiles, string l1Mbr_folder,
                                          string l2Mbr_folder) = 0;
                                          
   virtual ~MPILoadCalculator()
   {}
   
   // map<long, int, std::greater<long> >* createReducers(int cellCount, map<int, vector<Envelope*>* >* geomsByCellMap,
//  												 map<int, vector<Envelope*>* >* queriesByCellMap);

  long reduce(int cellId, vector<Envelope*>* col1, vector<Envelope*>* col2)
  {
    Index tree;
   
    if(col1 != nullptr && !col1->empty())
	 tree.createRTree(col1);

	map<Envelope*, vector<void *> >* intersectionMap = nullptr;	
	
	if(col2 != nullptr && !col2->empty())
      intersectionMap = tree.query(col2);
	
	Join spatialJoin;

    //list<pair<Envelope*, Envelope*> > *pairs = nullptr;
    long joinPairs = 0;
    if(intersectionMap != nullptr)
    {
        joinPairs = spatialJoin.join(intersectionMap);
        //cout<<joinPairs<<" "; 
    }
    
    return joinPairs;
    
    //cout<< "  "<<pairs->size() <<endl;
 }

 int readMBRFile(string path, vector<Envelope*> *envs)
 {
  int recordsRead = 0;
  
  ifstream myfile(path);
  double x1, y1, x2, y2;
  if (myfile.is_open())
  {
    	std::string line;
	
		while (std::getline(myfile, line))
		{
           stringstream env(line); 
           if (env >> x1 >> y1 >> x2 >> y2)
           {
             // Envelope (double x1, double x2, double y1, double y2)
             Envelope *env = new Envelope(x1, x2, y1, y2);
             envs->push_back(env);
             
             recordsRead++;
           }
        }
        myfile.close();
   }

   return recordsRead;
 }

 map<long, int, std::greater<long> >* createReducers(int cellCount, map<int, vector<Envelope*>* >* geomsByCellMap,
 												 map<int, vector<Envelope*>* >* queriesByCellMap)
 {   
     map<long, int, std::greater<long> > *loadMap = new map<long, int, std::greater<long> >();
     
     long totalOutput = 0;
     
     for(int cellId = 0; cellId < cellCount; cellId++) {
        if(geomsByCellMap->find(cellId) != geomsByCellMap->end() && 
        								queriesByCellMap->find(cellId) != queriesByCellMap->end()) {
           int output = reduce(cellId, geomsByCellMap->at(cellId), queriesByCellMap->at(cellId)); 
  
           if(output>0) {
             cout<< " ["<<cellId <<"] : "<<output<<endl;
             loadMap->insert(pair<long,int>(output, cellId));
           }
  
           totalOutput += output;
        }
     }
     //verify(totalOutput);

     return loadMap;
  }

};

#endif