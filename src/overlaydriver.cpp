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

using namespace std;

/*
 mpirun -np 2 ./mpiio 2 ../datasets/parks5k 
 1st arg is number of partitions
 2nd arg is 1st file
 3rd arg is 2nd file
*/

Config* init(int argc, char **argv)
{
  /*  cout<<"Number of arguments "<<argc<<endl;

	for(int i = 1; i<argc; i++) {
	  cout<< argv[i]<<endl;
	} */
	
	Config *args = new Config(argc, argv);
	
    return args;
}

void initMPI(int argc, char **argv, Config &args)
{
    int rank, MPI_Processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &MPI_Processes);

    args.numProcesses = MPI_Processes;
    args.rank = rank;
    
    if (argc < 3) {
        if (rank == 0) cout<<"Usage: mpirun -np #processes ./mpi_program #partitions directory1 directory2 "<<endl;
        MPI_Finalize();
        exit(1);
    }

}

int main1(int argc, char **argv) 
{
  Config *args = init(argc, argv);
  cout<<""<<args->getLayer1()->at(2)<<endl;
  cout<<""<<args->getLayer2()->at(2)<<endl;
  return 0;
}

int main(int argc, char **argv)
{	

	Config *args = init(argc, argv);
	
	initMPI(argc, argv, *args);
	
	double t1, t2;
	
	t1 = MPI_Wtime();
	
	FilePartitioner *partitioner = new MPI_File_Partitioner();

	partitioner->initialize(*args);
	//cout<<"init done"<<endl;
    
    pair<FileSplits*, FileSplits*> splitPair = partitioner->partition();
	
	Parser *parser = new WKTParser();
	
	cout<<"P"<<args->rank<<", lines, "<<splitPair.first->numLines()<<endl;
	
	list<Geometry*> *layer1Geoms = parser->parse(*splitPair.first, args->rank, args->numProcesses);


    //if(layer1Geoms == NULL)
    //    cout<<args->rank<<": NULL geometries "<<endl;
    //else
    	cout<<"P"<<args->rank<<", geoms, "<<layer1Geoms->size()<<endl;

	splitPair.first->clear();
    
    delete splitPair.first;
	
	Index rtree;
	rtree.createRTree(layer1Geoms);
	
	//rtree.createIndex(layer1Geoms);
	cout<<"P"<<args->rank<<", Index created "<<endl;
	
	FileSplits *layer2 = new FileSplits();
    
    ifstream in_stream;
    string line;
    
    in_stream.open(argv[3]);

    for( string line; getline( in_stream, line ); )
    {
      layer2->addGeom(line);
    }
	
	//cout<<"P"<<args->rank<<", Number of lines in layer2 "<<layer2->numLines()<<endl;
	
	Parser *parser2 = new WKTParser();
    
    list<Geometry*> *layer2Geoms = parser2->parse(*layer2, args->rank, args->numProcesses);
	//cout<<"P"<<args->rank<<" Layer 2 geoms, "<<layer2Geoms->size()<<endl;
	
	
	
	map<Geometry*, vector<void *> >* intersectionMap = rtree.query(layer2Geoms);
	
	//cout<<args->rank<<" : query result "<<intersectionMap->size()<<endl;
	
	
	map<Geometry*, vector<void *> >::iterator itOuter;
    int numOverlaps = 0;   
    for (itOuter = intersectionMap->begin(); itOuter != intersectionMap->end(); ++itOuter) {
           // std::cout << it->first << " => " << it->second->size() << '\n';
         numOverlaps += itOuter->second.size();  
    }
    cout<<args->rank<<", query result "<<numOverlaps<<endl;
    
    
    //Join spatialJoin;
    //list<pair<Geometry*, Geometry*> > *pairs = spatialJoin.join(intersectionMap);
    
    
    Overlay overlay;
    //overlay.initOutputFile(args->rank);
    list<Geometry*>* pairs = overlay.overlay(intersectionMap);	
    
    t2 = MPI_Wtime();	
    cout<<args->rank<<", Number of overlapping pairs, "<<pairs->size()<<", Time: ,"<<(t2-t1)<<endl<<endl;
    
    //overlay.closeFile();
    		
	/*Grid *grid = new UniformGrid(16);
	
    grid->setDimension(1,2,3,4);
    
	grid->populateGridCells(*layer1Geoms, true);
	
	grid->populateGridCells(*layer2Geoms, false);

	//shuffleExchange();
	*/
	partitioner->finalize();
	
	//delete parser;
	//delete partitioner;


   
//    cout<<"P"<<args->rank<<", Time: ,"<<(t2-t1)<<endl;

	MPI_Finalize();

	return 0;
}
