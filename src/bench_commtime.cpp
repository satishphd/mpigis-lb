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
#include "bufferManager/singleLayerGeomBufferMgr.h"

#include <fstream>
#include <unistd.h>

//#define DBUG2 2
//#define DBUG1 1

using namespace std;

/*
 mpirun -np 2 ./mpiio 2 ../datasets/parks5k 
 1st arg is number of partitions
 2nd arg is 1st file
 3rd arg is 2nd file
*/

int main(int argc, char **argv)
{	
    Config args(argc, argv); 
    
    args.initMPI(argc, argv);

	double total_t1, total_t2;
	double comm_t1, comm_t2;
	double parse_t1, parse_t2;
	double partition_t1, partition_t2;
	//t1 = MPI_Wtime();
	
	char hostname[256];
	gethostname(hostname,255);
	
	total_t1 = MPI_Wtime();
	parse_t1 = MPI_Wtime();
	
	MPI_File_Partitioner *partitioner = new MPI_File_Partitioner();

	MPI_File layer = partitioner->initializeLayer(args);
	
    FileSplits* split = partitioner->partitionLayer(layer);
    
    long numLines = split->numLines();
    long totalLines = 0;
  
    Parser *parser = new RoadNetworkParser();
    
    //Parser *parser = new WKTParser();
    
    list<Geometry*> *layer1Geoms = parser->parse(*split, args.rank, args.numProcesses);
    
    //cerr<<"P"<<args.rank<<" "<<hostname<<", geoms 1, "<<layer1Geoms->size()<<endl;
    
    parse_t2 = MPI_Wtime();
    
    partition_t1 = MPI_Wtime();

    Envelope mbr = GeometryUtility :: getMBR( layer1Geoms );
    
    SpatialTypes types;
  
    Envelope universe = types.reduceByUnion(&mbr);
   
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
	
	split->clear();
    
    delete split;
    
    MappingStrategy *strategy = new RoundRobinStrategy(args.numPartitions, args.numProcesses);
    
    map<int, Envelope*> *grid = uniGrid->getGridCellsMap();
    
    strategy->createCellProcessMap(grid);
    
    partition_t2 = MPI_Wtime();
    
			//     if(args.rank == 0) {
			//       strategy->printStrategy();
			//     }
    
		    //map<int, vector<int>* > *mapping = strategy->getProcessToCellsMap();
    
    comm_t1 = MPI_Wtime();
    
    		
	BufferRoadNetwork roadBufMgr(strategy, uniGrid, &args);
	map<int,list<RoadInfo*>* >*roadSegmentsByCell = roadBufMgr.shuffleExchangeByCell();
		     
    CollectiveAttributes* attr = roadBufMgr.getCollectiveAttributes();
		   
    comm_t2 = MPI_Wtime();
    total_t2 = MPI_Wtime();
    
    int comm_bufSize = attr->getSendBufSize() + attr->getRecvBufSize();
    
  	 //  MRDriver driver;
	 // 	driver.createReducers(&args, roadSegmentsByCell);
	
	
	double parse_time = (parse_t2 - parse_t1);
	double max_parse_time;
	MPI_Reduce(&parse_time, &max_parse_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	double partition_time = (partition_t2 - partition_t1);
	double max_partition_time;
	MPI_Reduce(&partition_time, &max_partition_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	double communication_time = (comm_t2 - comm_t1) ;
	double max_communication_time;
	MPI_Reduce(&communication_time, &max_communication_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	double total_timeByP = (total_t2 - total_t1);
	double total_time;
	MPI_Reduce(&total_timeByP, &total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	if(args.rank == 0) {
	  cout<<hostname<<", "<<args.rank<<",parse "<<max_parse_time<<", partition "<< max_partition_time
	  				<<", comm "<<max_communication_time<<", "<<total_time<<endl;
	}
	 
    //cout<<hostname<<", "<<args.rank<<", "<<parse_time<<", "<< partition_time<<", "<<communication_time
    //								<<", "<<(total_t2 - total_t1)<<", "<<attr->getSendBufSize()<<", "<<attr->getRecvBufSize()<<endl;
	
		 //   cout<<hostname<<":"<<args.rank<<", Parsing time, "<<(parse_t2 - parse_t1)<<endl;
	  	//  cout<<hostname<<":"<<args.rank<<", Partition time, "<<(partition_t2 - partition_t1)<<endl;
		//     cout<<hostname<<":"<<args.rank<<", Communication Time, "<<(comm_t2 - comm_t1)<<endl;
  		//	cout<<hostname<<":"<<args.rank<<", Total time, "<<(total_t2 - total_t1)<<endl;

	MPI_Finalize();

    return 0;
}

// this is for polygonal shape data 
int main10(int argc, char **argv)
{	
    Config args(argc, argv); 
    
    args.initMPI(argc, argv);

	double total_t1, total_t2;
	double comm_t1, comm_t2;
	double parse_t1, parse_t2;
	double partition_t1, partition_t2;
	//t1 = MPI_Wtime();
	
	char hostname[256];
	gethostname(hostname,255);
	
	total_t1 = MPI_Wtime();
	parse_t1 = MPI_Wtime();
	
	MPI_File_Partitioner *partitioner = new MPI_File_Partitioner();

	MPI_File layer = partitioner->initializeLayer(args);
	
    FileSplits* split = partitioner->partitionLayer(layer);
    
    long numLines = split->numLines();
    long totalLines = 0;
  
    //Parser *parser = new RoadNetworkParser();
    
    Parser *parser = new WKTParser();
    
    list<Geometry*> *layer1Geoms = parser->parse(*split, args.rank, args.numProcesses);
    
    //cerr<<"P"<<args.rank<<" "<<hostname<<", geoms 1, "<<layer1Geoms->size()<<endl;
    
    parse_t2 = MPI_Wtime();
    
    partition_t1 = MPI_Wtime();

    Envelope mbr = GeometryUtility :: getMBR( layer1Geoms );
    
    SpatialTypes types;
  
    Envelope universe = types.reduceByUnion(&mbr);
   
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
	
	split->clear();
    
    delete split;
    
    MappingStrategy *strategy = new RoundRobinStrategy(args.numPartitions, args.numProcesses);
    
    map<int, Envelope*> *grid = uniGrid->getGridCellsMap();
    
    strategy->createCellProcessMap(grid);
    
    partition_t2 = MPI_Wtime();
    
			//     if(args.rank == 0) {
			//       strategy->printStrategy();
			//     }
    
		    //map<int, vector<int>* > *mapping = strategy->getProcessToCellsMap();
    
    comm_t1 = MPI_Wtime();
    
    		/*
		     BufferRoadNetwork roadBufMgr(strategy, uniGrid, &args);
		     map<int,list<RoadInfo*>* >*roadSegmentsByCell = roadBufMgr.shuffleExchangeByCell();
		    */
		    
		    SingleLayerGeomBufferMgr singleBufMgr(strategy, uniGrid, &args);
		    
		    map<int, list<Geometry*>* >* geomMap = singleBufMgr.shuffleExchangeGrpByCell();
		    CollectiveAttrGeom *attr = singleBufMgr.getCollectiveAttributes();
		    
    		//MRDriver driver;
		    //driver.createReducers(&args, roadSegmentsByCell);
    
		    /*MPI_BufferManager manager(strategy, uniGrid, &args);
    
		      pair<map<int,vector<GeomInfo*>* > *, map<int,vector<GeomInfo*>* >* >* envelopesMapPair = 
    															manager.shuffleExchangeMBRGroupByCellId();
		    //cout<<args.rank<<": "<<"L1 env "<<envelopesMapPair->first->size()<<" L2 env "<<envelopesMapPair->second->size()<<endl;

		    MRDriver driver;
    
		    driver.createReducers(&args, envelopesMapPair);
    		*/
    
   		   /*	
		   BufferManagerForGeoms geomsBuffMgr(strategy, uniGrid, &args);
		   pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >*geomMapPair = 
  																	geomsBuffMgr.shuffleExchangeGrpByCell();
   		   */
   			/*
		    MRDriver driver;
		    driver.createReducers(&args, geomMapPair);
			*/

			//pair<list<Geometry*>*, list<Geometry*>* >* geomListPair = geomsBuffMgr.shuffleExchange();
		    //cout<<"Parsed Geoms ["<<args.rank<<"] "<<geomListPair->first->size()<<" "<<geomListPair->second->size()<<endl;
    
    
			//driver.createReducers(&args, geomMapPair);
 

    comm_t2 = MPI_Wtime();
    total_t2 = MPI_Wtime();
    
    int comm_bufSize = attr->getSendBufSize() + attr->getRecvBufSize();
    
    double parse_time = (parse_t2 - parse_t1);
	double max_parse_time;
	MPI_Reduce(&parse_time, &max_parse_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	double partition_time = (partition_t2 - partition_t1);
	double max_partition_time;
	MPI_Reduce(&partition_time, &max_partition_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	double communication_time = (comm_t2 - comm_t1) ;
	double max_communication_time;
	MPI_Reduce(&communication_time, &max_communication_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	double total_timeByP = (total_t2 - total_t1);
	double total_time;
	MPI_Reduce(&total_timeByP, &total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	if(args.rank == 0) {
	  cout<<hostname<<", "<<args.rank<<",parse "<<max_parse_time<<", partition "<< max_partition_time
	  				<<", comm "<<max_communication_time<<", "<<total_time<<endl;
	}
	
	MPI_Finalize();

    return 0;
}

	// cout<<hostname<<", "<<args.rank<<", "<<(parse_t2 - parse_t1)<<", "<< (partition_t2 - partition_t1)<<", "<<(comm_t2 - comm_t1) 
	//     								<<", "<<(total_t2 - total_t1)<<", "<<comm_bufSize<<endl;
	 //   cout<<hostname<<":"<<args.rank<<", Parsing time, "<<(parse_t2 - parse_t1)<<endl;
	  	//  cout<<hostname<<":"<<args.rank<<", Partition time, "<<(partition_t2 - partition_t1)<<endl;
		//     cout<<hostname<<":"<<args.rank<<", Communication Time, "<<(comm_t2 - comm_t1)<<endl;
  		//	cout<<hostname<<":"<<args.rank<<", Total time, "<<(total_t2 - total_t1)<<endl;
  		
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
	
	list<Geometry*> *layer1Geoms = parser->parse(*splitPair.first, args.rank, args.numProcesses);

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
