#include <string>
#include <list>
#include "filePartition/FilePartitioner.h"
#include "filePartition/MPI_File_Partitioner.h"
#include "filePartition/MPI_LargeFile_Partitioner.h"

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

#include "bufferManager/singleLayerGeomBufferMgr.h"
#include "geom_util/fileRead.h"

#include "bufferManager/bufferManagerGeoms.h"
#include "mapreduce/mrdriver.h"

#include "bufferManager/bufferRoadnetwork.h"
#include "bufferManager/MultiRoundOneLayerGeomBufferMgr.h"

#include "bufferManager/AllObjectsCommBuffer_DerivedType.h"

#include <fstream>
#include <unistd.h>

//#define DBUG2 2
#define DBUG1 1

using namespace std;

int main0(int argc, char **argv)
{
   Config args(argc, argv); 
    
   args.initMPI(argc, argv);
   
   FilePartitioner *partitioner = new MPI_File_Partitioner();

   partitioner->initialize(args);
   cout<<"init done"<<endl;
    
   pair<FileSplits*, FileSplits*> splitPair = partitioner->partition();
   cout<<"Partition done"<<endl;
   
   Parser *parser = new RoadNetworkParser();
   
   //list<Geometry*> *layer1Geoms = parser->parse(*splitPair.first, args.rank, args.numProcesses);
   //cout<<"Number of geoms "<<layer1Geoms->size()<<endl;
   FileSplits* split = splitPair.first;
   cout<<"File size "<<split->numLines()<<endl;
   MPI_Finalize();
   
   return 0;
}

/*
 mpirun -np 2 ./mpiio 2 ../datasets/parks5k 
 1st arg is number of partitions
 2nd arg is 1st file
 3rd arg is 2nd file
*/

int main10(int argc, char **argv) 
{	
    Config args(argc, argv); 
    
    args.initMPI(argc, argv);

    double total_t1, total_t2;
	double comm_t1, comm_t2;
	double parse_t1, parse_t2;
	double partition_t1, partition_t2;
	double compute_t1, compute_t2;
	
	FileReader samplesForQuery;
	//list<Geometry*> *queryGeoms = samplesForQuery.readSampleFile("rangeQuerySamples/samples_cemetery_10k.txt", &args);
	
	//list<Geometry*> *queryGeoms = samplesForQuery.readSampleFile("rangeQuerySamples/samples_cemetery_1k.txt", &args);
	//list<Geometry*> *queryGeoms = samplesForQuery.readSampleFile("rangeQuerySamples/cemetery_samples_100.txt", &args);
	
	// if(NULL == queryGeoms)
// 	    MPI_Abort(MPI_COMM_WORLD, 911);
// 	double t1, t2;
	
	//t1 = MPI_Wtime();
	
	char hostname[256];
	gethostname(hostname,255);
	
	parse_t1 = MPI_Wtime();
	
			#ifdef DBUG2   
		    string fileStr = "debug_logs/" + args.log_file + to_string(args.rank);
		    char *filename = (char *)fileStr.c_str();
	
			std::ofstream ofs;
		    ofs.open (filename, std::ofstream::out | std::ofstream::app);
			#endif
	
	FilePartitioner *partitioner = new MPI_LargeFile_Partitioner();
    //FilePartitioner *partitioner = new MPI_File_Partitioner();
	
	partitioner->initialize(args);
	//cout<<"init done"<<endl;
    
    pair<FileSplits*, FileSplits*> splitPair = partitioner->partition();
    
    //cerr<<"P"<<args.rank<<" lines, "<<splitPair.first->numLines()<<endl;
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
    
    list<Geometry*> *layer1Geoms = parser->parse(*splitPair.first, args.rank, args.numProcesses);
    
    list<Geometry*> *layer2Geoms = parser->parse(*splitPair.second, args.rank, args.numProcesses);
    
    //cerr<<"P"<<args.rank<<" "<<hostname<<", geoms 1, "<<layer1Geoms->size()<<endl;
    
   		 //#ifdef DBUG1  
    		long numGeoms = layer1Geoms->size();
    		long totalGeoms = 0;
    
		    MPI_Allreduce(&numGeoms, &totalGeoms, 1, MPI_LONG, MPI_SUM, MPI_COMM_WORLD);
    
		    if(args.rank == 0)
		       cerr<<"total number of Geoms layer 1 "<<totalGeoms<<endl; 
	//    #endif
    
    //parse_t2 = MPI_Wtime();
   
  //cout<<"parsing time "<<parse_t2 - parse_t1<<endl;
    
  return 0;
}

int main(int argc, char **argv)
{	
    Config args(argc, argv); 
    
    args.initMPI(argc, argv);

    double total_t1, total_t2;
	double comm_t1, comm_t2;
	double parse_t1, parse_t2;
	double partition_t1, partition_t2;
	double compute_t1, compute_t2;
	
	//FileReader samplesForQuery;
	//list<Geometry*> *queryGeoms = samplesForQuery.readSampleFile("rangeQuerySamples/samples_cemetery_10k.txt", &args);
	
	//list<Geometry*> *queryGeoms = samplesForQuery.readSampleFile("rangeQuerySamples/samples_cemetery_1k.txt", &args);
	//list<Geometry*> *queryGeoms = samplesForQuery.readSampleFile("rangeQuerySamples/cemetery_samples_100.txt", &args);
	
	//if(NULL == queryGeoms)
	//    MPI_Abort(MPI_COMM_WORLD, 911);
	double t1, t2;
	
	//t1 = MPI_Wtime();
	
	char hostname[256];
	gethostname(hostname,255);
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	parse_t1 = MPI_Wtime();
	
			#ifdef DBUG2   
		    string fileStr = "debug_logs/" + args.log_file + to_string(args.rank);
		    char *filename = (char *)fileStr.c_str();
	
			std::ofstream ofs;
		    ofs.open (filename, std::ofstream::out | std::ofstream::app);
			#endif
	
	FilePartitioner *partitioner = new MPI_LargeFile_Partitioner();
    //FilePartitioner *partitioner = new MPI_File_Partitioner();
	
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
    
    list<Geometry*> *layer2Geoms = parser->parse(*splitPair.second);
    
    //cerr<<"P"<<args.rank<<" "<<hostname<<", geoms 1, "<<layer1Geoms->size()<<endl;
    
   		 #ifdef DBUG1  
    		long numGeoms = layer1Geoms->size();
    		long totalGeoms = 0;
    
		    MPI_Reduce(&numGeoms, &totalGeoms, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
		    if(args.rank == 0)
		       cerr<<"total number of Geoms layer 1 "<<totalGeoms<<endl; 
	    #endif
    
    parse_t2 = MPI_Wtime();
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    partition_t1 = MPI_Wtime();
    
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

    //cout<<"PopulateGridCells layer A "<<args.rank<<endl;
    
    uniGrid->populateGridCells(layer2Geoms, false);

    //cout<<"PopulateGridCells layer B "<<args.rank<<endl;
   
    //if(args.rank == 0) {
     //list<string> logs = uniGrid->localGridStatistics(); 
    //}

    
//     int *outputBuffer = uniGrid->numShapesPerCell(); 
// 
//     if(args.rank == 0) {
//      
//       map<int, pair<int, int> > cellMap = uniGrid->globalGridStatistics(outputBuffer);
//       uniGrid->printGlobalGridStatistics(cellMap);
//     }
    
    
   //  #ifdef DBUG2   
//     for(string s : logs)
//       ofs<<s<<endl;
// 
// 	ofs.close();
// 	#endif
	
	splitPair.first->clear();
    
    delete splitPair.first;
    
    splitPair.second->clear();
    
    delete splitPair.second;
    
    
    MappingStrategy *strategy = new RoundRobinStrategy(args.numPartitions, args.numProcesses);
    
    map<int, Envelope*> *grid = uniGrid->getGridCellsMap();
    
    strategy->createCellProcessMap(grid);
    
    partition_t2 = MPI_Wtime();
    
//     if(args.rank == 0) {
//       strategy->printStrategy();
//     }
    
    //map<int, vector<int>* > *mapping = strategy->getProcessToCellsMap();
   
   /*
   if(args.rank == 0) {
      cout<<"Multi-Round"<<endl;
   }
   
   MultiRoundOneLayerGeomBufferMgr geomsBuffMgr(strategy, uniGrid, &args);
   map<int, list<Geometry*>* >* geomsByCellMap = geomsBuffMgr.shuffleExchangeGrpByCell();
   
   MRDriver driver;
   driver.createReducers(&args, geomsByCellMap, queryGeoms);
   */
    
    if(args.rank == 0) {
      //cout<<"Single-Round"<<endl;
   }
   /*
    comm_t1 = MPI_Wtime();
 
    SingleLayerGeomBufferMgr geomsBuffMgr(strategy, uniGrid, &args);
    map<int, list<Geometry*>* >* geomsByCellMap = geomsBuffMgr.shuffleExchangeGrpByCell();
   
    comm_t2 = MPI_Wtime();
    //total_t2 = MPI_Wtime();
    
    compute_t1 = MPI_Wtime();
    
    MRDriver driver;
    driver.createReducers(&args, geomsByCellMap, queryGeoms);
   
    compute_t2 = MPI_Wtime(); 
 */
 
    /*
    BufferRoadNetwork roadBufMgr(strategy, uniGrid, &args);
    map<int,list<RoadInfo*>* >*roadSegmentsByCell = roadBufMgr.shuffleExchangeByCell();
    MRDriver driver;
    
    driver.createReducers(&args, roadSegmentsByCell);
    */
    
    /*MPI_BufferManager manager(strategy, uniGrid, &args);
    
      pair<map<int,vector<GeomInfo*>* > *, map<int,vector<GeomInfo*>* >* >* envelopesMapPair = 
    															manager.shuffleExchangeMBRGroupByCellId();
    //cout<<args.rank<<": "<<"L1 env "<<envelopesMapPair->first->size()<<" L2 env "<<envelopesMapPair->second->size()<<endl;

    MRDriver driver;
    
    driver.createReducers(&args, envelopesMapPair);
    */
   
   comm_t1 = MPI_Wtime();
  
   // BufferManagerForGeoms geomsBuffMgr(strategy, uniGrid, &args);
//    pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >*geomMapPair = 
//   																	geomsBuffMgr.shuffleExchangeGrpByCell();
   AllObjectsCommBuffer geomsBuffMgr(strategy, uniGrid, &args);
   map<int, list<Geometry*>* >* geomsByCellMap = geomsBuffMgr.shuffleExchangeGrpByCell();
   
   
    comm_t2 = MPI_Wtime();

    compute_t1 = MPI_Wtime();
  
   // MRDriver driver;
    //driver.createReducers(&args, geomMapPair);
	
 	compute_t2 = MPI_Wtime(); 
	//pair<list<Geometry*>*, list<Geometry*>* >* geomListPair = geomsBuffMgr.shuffleExchange();
    //cout<<"Parsed Geoms ["<<args.rank<<"] "<<geomListPair->first->size()<<" "<<geomListPair->second->size()<<endl;
    
    
	//driver.createReducers(&args, geomMapPair);
 
	/*
	Index tree;
	tree.createRTree(layer1Geoms);
	//tree.createQuadTree(layer1Geoms);
	
	
	//cout<<args->rank<<", Index created "<<endl;
	
	FileSplits *layer2 = new FileSplits();
    
    ifstream in_stream;
    string line;
    
    in_stream.open(argv[3]);

    for( string line; getline( in_stream, line ); )
    {
      layer2->addGeom(line);
    }
	
	//cout<<"P"<<args->rank<<" Number of lines in layer2 "<<layer2->numLines()<<endl;
	
	Parser *parser2 = new WKTParser();
    
    list<Geometry*> *layer2Geoms = parser2->parse(*layer2, args->rank, args->numProcesses);
	//cout<<"P"<<args->rank<<" Layer 2 geoms, "<<layer2Geoms->size()<<endl;
	
	
	map<Geometry*, vector<void *> >* intersectionMap = tree.query(layer2Geoms);
	
	//if(intersectionMap!=nullptr)
    //	cerr<<args->rank<<" : query result "<<intersectionMap->size()<<endl;
	
	
	map<Geometry*, vector<void *> >::iterator itOuter;
    int numOverlaps = 0;   
    for (itOuter = intersectionMap->begin(); itOuter != intersectionMap->end(); ++itOuter) {
           // std::cout << it->first << " => " << it->second->size() << '\n';
         numOverlaps += itOuter->second.size();  
    }
    cerr<<args->rank<<", query result, "<<numOverlaps<<endl;
    
    long numPairs = numOverlaps;
    long totalPairs = 0;
    
 //    MPI_Reduce(&numPairs, &totalPairs, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
//     
//     if(args->rank == 0)
//       cerr<<"total number of Output "<<totalPairs<<endl;
*/    
    //t2 = MPI_Wtime();

	double parse_time = (parse_t2 - parse_t1);
    double partition_time = (partition_t2 - partition_t1);
    double communication_time = (comm_t2 - comm_t1) ;
    double compute_time = (compute_t2 - compute_t1);
    
    printf("%d %.2f %.2f %.2f %.2f \n ", args.rank, parse_time, partition_time, communication_time, compute_time); 
    fflush(stdout);
    //cout<<hostname<<":"<<args.rank<<", Time, "<<(t2-t1)<<endl;

	MPI_Finalize();

    return 0;
}



int main1(int argc, char **argv)
{	

	Config args(argc, argv);
	
	args.initMPI(argc, argv);
	
	double t1, t2;
	
	t1 = MPI_Wtime();
	
	FilePartitioner *partitioner = new MPI_File_Partitioner();

	partitioner->initialize(args);
	//cout<<"init done"<<endl;
    
    pair<FileSplits*, FileSplits*> splitPair = partitioner->partition();

	Parser *parser = new WKTParser();
	
	cout<<"P"<<args.rank<<" lines, "<<splitPair.first->numLines()<<endl;
	
	list<Geometry*> *layer1Geoms = parser->parse(*splitPair.first);

    //Overlay overlay;
    //overlay.initOutputFile(args->rank);

    //overlay.writeGeoms(layer1Geoms);

    //overlay.closeFile();
    
    //if(layer1Geoms == NULL)
    //    cout<<args->rank<<": NULL geometries "<<endl;
    //else
    	cout<<"P"<<args.rank<<", geoms, "<<layer1Geoms->size()<<endl;

	splitPair.first->clear();
    
    delete splitPair.first;
	
	
	Index tree;
	tree.createRTree(layer1Geoms);
	
	//tree.createIndex(layer1Geoms);
	cout<<args.rank<<", Index created "<<endl;
	
	FileSplits *layer2 = new FileSplits();
    
    ifstream in_stream;
    string line;
    
    in_stream.open(argv[3]);

    for( string line; getline( in_stream, line ); )
    {
      layer2->addGeom(line);
    }
	
	//cout<<"P"<<args->rank<<" Number of lines in layer2 "<<layer2->numLines()<<endl;
	
	Parser *parser2 = new WKTParser();
    
    list<Geometry*> *layer2Geoms = parser2->parse(*layer2, args.rank, args.numProcesses);
	cout<<"P"<<args.rank<<" Layer 2 geoms, "<<layer2Geoms->size()<<endl;
	
	
	map<Geometry*, vector<void *> >* intersectionMap = tree.query(layer2Geoms);
	
	//if(intersectionMap!=nullptr)
    //	cerr<<args->rank<<" : query result "<<intersectionMap->size()<<endl;
	
	
	map<Geometry*, vector<void *> >::iterator itOuter;
    int numOverlaps = 0;   
    for (itOuter = intersectionMap->begin(); itOuter != intersectionMap->end(); ++itOuter) {
           // std::cout << it->first << " => " << it->second->size() << '\n';
         numOverlaps += itOuter->second.size();  
    }
    cerr<<args.rank<<", query result, "<<numOverlaps<<endl;
    
    Join spatialJoin;

    list<pair<Geometry*, Geometry*> > *pairs = spatialJoin.join(intersectionMap);
    
    /*Overlay overlay;
    list<pair<Geometry*, Geometry*> > *pairs = overlay.overlay(intersectionMap);	
    */
	
    //cout<<args->rank<<", Number of overlapping pairs "<<pairs->size()<<endl;
		
	/*Grid *grid = new UniformGrid(16);
	
    grid->setDimension(1,2,3,4);
    
	grid->populateGridCells(*layer1Geoms, true);
	
	grid->populateGridCells(*layer2Geoms, false);

	//shuffleExchange();
	*/
	partitioner->finalize();
	
	//delete parser;
	//delete partitioner;

    t2 = MPI_Wtime();
   
    cout<<"Time ,"<<args.rank<<", "<<(t2-t1)<<endl;

	MPI_Finalize();

	return 0;
}
