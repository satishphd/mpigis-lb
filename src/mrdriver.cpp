#include "mapreduce/mrdriver.h"


void  MRDriver :: createReducers(Config *args, map<int, Cell*> *shapesInCell)
{ 
    int cellCount = args->numPartitions;
    
    Reducer *reducer = new GeomJoinReducer();
    
    int totalOutput = 0;
     
    for(int cellId = 0; cellId < cellCount; cellId++) {
       if(shapesInCell->find(cellId) != shapesInCell->end()) {
		    Cell *cell = shapesInCell->at(cellId);
       		
       		list<Geometry*>* col1 = cell->getLayerAGeom();

       		if(col1 != NULL && col1->empty())
				continue;
				       		
       		list<Geometry*>* col2 = cell->getLayerBGeom();
       		
       		if(col2 != NULL && col2->empty())
       			continue;
       			
       		int output = reducer->reduce(cellId, col1, col2);
       		
       		totalOutput += output;
       		
       		if(output>0)
             cout<< " ["<<cellId <<"] : "<<output<<endl;
       }
       
    }
    verify(totalOutput);
}

// This is for printing purposes
void  MRDriver :: createReducers(Config *args, map<int, list<Geometry*>* >* geomsByCellMap, list<Geometry*>* queries)
 { 
   /*  int cellCount = args->numPartitions;
     
     Reducer *reducer = new GeomPrintReducer();
     
     int totalOutput = 0;
     
     for(int cellId = 0; cellId < cellCount; cellId++) {
        if(geomsByCellMap->find(cellId) != geomsByCellMap->end() && queries != NULL) {
           int output = reducer->reduce(cellId, geomsByCellMap->at(cellId), queries); 
  
           //if(output>0)
             //cout<< " ["<<cellId <<"] : "<<output<<endl;
  
           totalOutput += output;
        }
     } */
     //verify(totalOutput);
 } 

/*
 void  MRDriver :: createReducers(Config *args, map<int, list<Geometry*>* >* geomsByCellMap, list<Geometry*>* queries)
 { 
     int cellCount = args->numPartitions;
     
     Reducer *reducer = new GeomJoinReducer();
     
     int totalOutput = 0;
     
     for(int cellId = 0; cellId < cellCount; cellId++) {
        if(geomsByCellMap->find(cellId) != geomsByCellMap->end() && queries != NULL) {
           int output = reducer->reduce(cellId, geomsByCellMap->at(cellId), queries); 
  
           //if(output>0)
             //cout<< " ["<<cellId <<"] : "<<output<<endl;
  
           totalOutput += output;
        }
     }
     //verify(totalOutput);
 } */
 
 
 void  MRDriver :: createReducers(Config *args, map<int, list<Geometry*>* >* geomsByCellMap,
 												 map<int, list<Geometry*>* >* queriesByCellMap)
 { 
     int cellCount = args->numPartitions;
     
     Reducer *reducer = new GeomJoinReducer();
     
     int totalOutput = 0;
     
     for(int cellId = 0; cellId < cellCount; cellId++) {
        if(geomsByCellMap->find(cellId) != geomsByCellMap->end() && 
        								queriesByCellMap->find(cellId) != queriesByCellMap->end()) {
           int output = reducer->reduce(cellId, geomsByCellMap->at(cellId), queriesByCellMap->at(cellId)); 
  
           //if(output>0)
             //cout<< " ["<<cellId <<"] : "<<output<<endl;
  
           totalOutput += output;
        }
     }
     //verify(totalOutput);
 }

 void  MRDriver :: createReducers(Config *args, map<int,list<RoadInfo*>* >* roadLayer)
 {
	 int cellCount = args->numPartitions;

	 for(int cellId = 0; cellId < cellCount; cellId++) {
	 	if(roadLayer->find(cellId) != roadLayer->end()) {
	 		list<RoadInfo*>* envelopes = roadLayer->at(cellId);
	 		cout<<" [ "<<cellId<<" ] : "<<envelopes->size();
	 	}
	 }
 }

 void MRDriver :: createReducers(Config *args, pair<map<int,list<Geometry*>* > *, map<int,list<Geometry*>* >* >* cellPairMap)
 {
 	 int cellCount = args->numPartitions;
     map<int, list<Geometry*>* > *layer1Map = cellPairMap->first;
     map<int, list<Geometry*>* > *layer2Map = cellPairMap->second;
     
     Reducer *reducer = new GeomJoinReducer();
     
     int totalOutput = 0;
     
     for(int cellId = 0; cellId < cellCount; cellId++) {
        if(layer1Map->find(cellId) != layer1Map->end() && layer2Map->find(cellId) != layer2Map->end()) {
           int output = reducer->reduce(cellId, layer1Map->at(cellId), layer2Map->at(cellId)); 
           //cout<< " ["<<cellId <<"] : "<<output<<endl;
           totalOutput += output;
        }
     }
      verify(totalOutput);
 }

 void MRDriver :: createReducers(Config *args, pair<map<int,vector<GeomInfo*>* > *, map<int,vector<GeomInfo*>* >* >* cellPairMap)
 {
   
     int cellCount = args->numPartitions;
     map<int,vector<GeomInfo*>* > *layer1Map = cellPairMap->first;
     map<int,vector<GeomInfo*>* > *layer2Map = cellPairMap->second;
     
     /*Reducer *reducer = new EnvelopeReducer();
     
     int totalOutput = 0;
     
     for(int cellId = 0; cellId < cellCount; cellId++) {
        if(layer1Map->find(cellId) != layer1Map->end() && layer2Map->find(cellId) != layer2Map->end()) {
           int output = reducer->reduce(cellId, layer1Map->at(cellId), layer2Map->at(cellId)); 
           cout<< " ["<<cellId <<"] : "<<output<<endl;
           totalOutput += output;
        }
     } */
    // verify(totalOutput);
    
 }
 // 
//  MPI_Allreduce(
//     void* send_data,
//     void* recv_data,
//     int count,
//     MPI_Datatype datatype,
//     MPI_Op op,
//     MPI_Comm communicator)

  void MRDriver :: verify(int output)
  {
    int sumTotal = 0;
    MPI_Allreduce(&output, &sumTotal, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0)
	    cout<<"Total outputs (allreduce) " << sumTotal<<endl;
  }
 
