#ifndef __MRDRIVER_H_INCLUDED__
#define __MRDRIVER_H_INCLUDED__

#include "../spatialPartition/cell.h"
#include "envreducer.h"
#include "geomreducer.h"
#include "geomprintreducer.h"

#include "../filePartition/config.h"

class MRDriver
{
   void verify(int output);
   
   public:
   
   // map<int, pair<map<int,vector<Envelope*>* > *, map<int,vector<Envelope*>* > *>>
   void createReducers(Config *args, pair<map<int,vector<GeomInfo*>* > *, map<int,vector<GeomInfo*>* >* >* cellPairMap);
   
   void createReducers(Config *args, pair<map<int,list<Geometry*>* > *, map<int,list<Geometry*>* >* >* cellPairMap);
	
   void createReducers(Config *args, map<int, Cell*> *shapesInCell);	
   
   void createReducers(Config *args, map<int,list<RoadInfo*>* >* roadLayer);	
   
   void createReducers(Config *args, map<int, list<Geometry*>* >* geomsByCellMap, list<Geometry*>* queries);
   
   void createReducers(Config *args, map<int, list<Geometry*>* >* geomsByCellMap, map<int, list<Geometry*>* >* queriesByCellMap);
};

#endif 