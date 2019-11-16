#ifndef __MPILOADBALANCER_H_INCLUDED__
#define __MPILOADBALANCER_H_INCLUDED__

#include<iostream>
#include<fstream>
#include<sstream>
#include<stdio.h>
#include<string>
#include<map>

#include<list>
#include <stdlib.h>
#include <sys/time.h>
#include <geos/io/WKTReader.h>

#include<cstring>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>

#include "../geom_util/fileRead.h"
#include "../index/index.h"
#include "../join/join.h"

#include "staticMPILoadCalculator.h"
#include "dynamicMPILoadCalculator.h"

using namespace std;
using namespace geos::geom;

//long calcLoad();
class LoadBalancer
{
    int numFiles;
    string l1Folder;
    string l1MBRFolder;
  
    string l2Folder;
    string l2MBRFolder;

  public:
  
  int getNumFiles()
  {
     return numFiles;
  }
  
  string getL1Folder()
  {
    return l1Folder;
  }
   
  string getL2Folder()
  {
    return l2Folder;
  }
  
  string getL1MBRFolder()
  {
    return l1MBRFolder;
  }
  
  string getL2MBRFolder()
  {
    return l2MBRFolder;
  }
  
  LoadBalancer(int argc, char *argv[]);
  
  int refineOneCell(char *fileLayer1, char *fileLayer2);
  
  map<long, int, std::greater<long> >* seqCalculateLoad(int numFiles);
 
  map<long, int, std::greater<long>>* distributedLoadCalc(int numFiles);
  
  int masterRefinementScheduler(int nProcs, map<long, int, std::greater<long> >* loadByCell);
  
  void masterWithoutLoadCalc(int numFiles);
  
 // map<long, int, std::greater<long> >* mpiStaticLoadCalculator(int numFiles);
  
 // static map<long, int, std::greater<long> >* mpiDynLoadCalculator(int numFiles);
  
  void slaveTaskForRefinement();
  
  long reduce(int cellId, list<Envelope*>* col1, list<Envelope*>* col2);

  map<long, int, std::greater<long>>* createReducers(int cellCount, map<int, list<Envelope*>* >* geomsByCellMap,
 								   map<int, list<Envelope*>* >* queriesByCellMap);
 								   
};

#endif