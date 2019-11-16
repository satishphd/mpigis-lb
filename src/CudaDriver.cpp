#include <string>
#include <list>
#include "filePartition/FilePartitioner.h"
#include "filePartition/MPI_File_Partitioner.h"
#include "filePartition/FileSplits.h"
#include "parser/Parser.h"
#include "parser/WKTParser.h"
#include "filePartition/config.h"
#include <geos/geom/Geometry.h>
#include "spatialPartition/grid.h"
#include "spatialPartition/uniformGrid.h"
#include "index/index.h"
#include "join/join.h"
#include "overlay/overlay.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

#include "mpiTypes/mpitype.h"
#include "geom_util/util.h"
#include "parser/road_network_parser.h"
#include "spatialPartition/uniformGrid.h"
#include "spatialPartition/RtreeStructure.h"
#include "taskMap/TaskMap.h"
#include "taskMap/roundRobinTaskMap.h"

#include "bufferManager/bufferManagerGeoms.h"
#include "mapreduce/mrdriver.h"

#include "bufferManager/bufferRoadnetwork.h"

#include <fstream>
#include <unistd.h>

#include "cuda/CudaJoinInterface.h"

//#define DBUG2 2
//#define DBUG1 1

using namespace std;

/*
 mpirun -np 2 ./mpiio 2 ../datasets/parks5k 
 1st arg is number of partitions
 2nd arg is 1st file
 3rd arg is 2nd file
*/


int main2(int argc, char **argv) 
{
  Config args(argc, argv); 
  args.initMPI(argc, argv);
  
  char hostname[256];
  gethostname(hostname,255);
  cout<<hostname<<endl;
  //cout<<""<<args.getLayer1()->at(2)<<endl;
  //cout<<""<<args.getLayer2()->at(2)<<endl;
  return 0;
}

int main(int argc, char **argv)
{	
    Config args(argc, argv); 
    
    args.initMPI(argc, argv);

	double t1, t2;
	
	t1 = MPI_Wtime();
	
	char hostname[256];
	gethostname(hostname,255);
	
	#ifdef DBUG2   
    string fileStr = "debug_logs/" + args.log_file + to_string(args.rank);
    char *filename = (char *)fileStr.c_str();
	
	std::ofstream ofs;
    ofs.open (filename, std::ofstream::out | std::ofstream::app);
	#endif
	
	FilePartitioner *partitioner = new MPI_File_Partitioner();

	partitioner->initialize(args);
	//cout<<"init done"<<endl;
    
    pair<FileSplits*, FileSplits*> splitPair = partitioner->partition();
    
    //cerr<<"P"<<args->rank<<" lines, "<<splitPair.first->numLines()<<endl;
    //long numLines = splitPair.second->numLines();
    long numLines = splitPair.first->numLines();
    long totalLines = 0;
 
    #ifdef DBUG1   
    //MPI_Reduce(void* send_data, void* recv_data, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm communicator)
    MPI_Reduce(&numLines, &totalLines, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if(args.rank == 0)
       cerr<<"total number of lines "<<totalLines<<endl;
    #endif    
    
    //Parser *parser = new RoadNetworkParser();
    Parser *parser = new WKTParser();
    
    list<Geometry*> *layer1Geoms = parser->parse(*splitPair.first);
    cerr<<"P"<<args.rank<<" "<<hostname<<", geoms 1, "<<layer1Geoms->size()<<endl;
    
    list<Geometry*> *layer2Geoms = parser->parse(*splitPair.second);
    cerr<<"P"<<args.rank<<" "<<hostname<<", geoms 2, "<<layer2Geoms->size()<<endl;
    
    #ifdef DBUG1  
    long numGeoms = layer2Geoms->size();
    long totalGeoms = 0;
    
    MPI_Reduce(&numGeoms, &totalGeoms, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if(args.rank == 0)
       cerr<<"total number of Geoms layer 2 "<<totalGeoms<<endl; 
    #endif
    
    Envelope mbr = GeometryUtility :: getMBR( layer1Geoms );
    
    #ifdef DBUG2 
    //cerr<<args.rank<<", "<<mbr.toString();
    //cerr<<args.rank<<", "<<mbr.getMinX()<<" - "<<mbr.getMaxX()<<" , "<<mbr.getMinY()<<" - "<<mbr.getMaxY();
    
    //printf("%d, (%f %f), (%f %f) \n",args.rank, mbr.getMinX(), mbr.getMinY(), mbr.getMaxX(), mbr.getMaxY());
    //fflush(stdout);
    
    ofs <<"Rank " <<args.rank <<": local Envelope minX " << mbr.getMinX() <<" minY "<<mbr.getMinY()<<" maxX "<< mbr.getMaxX()<<" maxY "<< mbr.getMaxY()<<"\n";
    #endif
    
    SpatialTypes types;
  
    Envelope universe = types.reduceByUnion(&mbr);
   // cout<<"*****************************"<<endl;
   
    if(args.rank == 0)
       printf("Universe: %d, (%f %f), (%f %f) \n",0, universe.getMinX(), universe.getMinY(), universe.getMaxX() , universe.getMaxY());
    
    
    Grid *uniGrid = new UniformGrid(args.numPartitions, &universe);
    
//     if(args.rank == 0) {
//       cout<<"Number of cells in the grid "<<uniGrid->size()<<endl;
//       
//       uniGrid->printGridCoordinates();
//     }
    
    uniGrid->populateGridCells(layer1Geoms, true);

    cout<<"PopulateGridCells layer A "<<args.rank<<endl;
    
    uniGrid->populateGridCells(layer2Geoms, false);

    cout<<"PopulateGridCells layer B "<<args.rank<<endl;
   
    if(args.rank == 0) {
     //list<string> logs = uniGrid->localGridStatistics(); 
    }
	
	splitPair.first->clear();
    
    delete splitPair.first;
    
    splitPair.second->clear();
    
    delete splitPair.second;
    
    
    MappingStrategy *strategy = new RoundRobinStrategy(args.numPartitions, args.numProcesses);
    
    map<int, Envelope*> *grid = uniGrid->getGridCellsMap();
    
    strategy->createCellProcessMap(grid);
    
//     if(args.rank == 0) {
//       strategy->printStrategy();
//     }
    
    //map<int, vector<int>* > *mapping = strategy->getProcessToCellsMap();
    BufferManagerForGeoms geomsBuffMgr(strategy, uniGrid, &args);
    
    pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >*geomMapPair = 
  															geomsBuffMgr.shuffleExchangeGrpByCell();
    cerr<<"Shuffle-Exchange"<<endl;  															

    CudaJoinInterface cudaInterface;															
   
    cudaInterface.createReducers(&args, geomMapPair);
   
    t2 = MPI_Wtime();
   
    cout<<hostname<<":"<<args.rank<<", Time, "<<(t2-t1)<<endl;

	MPI_Finalize();

    return 0;
}
