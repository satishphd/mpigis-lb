#include <mpi.h>
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
#include "parser/pointNodeParser.h"

#include <fstream>
#include <unistd.h>

#define WORKTAG     1
#define DIETAG     2

void slave(string l1Folder, string l2Folder);
int processRefinement(char *fileLayer1, char *fileLayer2);
int masterWithOutLoadCalc(int numFiles);


// mpirun -np 16 ./lbDynamic 1024 ~/mpigis/indexed_data/sports/1024Parts/ ~/mpigis/indexed_data/cemetery/1024Parts/
// make lbDynamic 
int main(int argc, char *argv[])
{
	int         myrank;
	MPI_Init(&argc, &argv);   
	MPI_Comm_rank(MPI_COMM_WORLD,  &myrank);     
    
    double total_t1;
    
    if(argc < 4)
    {
      cout<<" Num files L1Folder L2Folder L1MBRFolder L2MBRFolder"<<endl;
      MPI_Abort(MPI_COMM_WORLD, myrank);
    }
      
   int numFiles = atoi(argv[1]);
   
   string l1Folder = string(argv[2]);
   string l2Folder = string(argv[3]);
	
	//char hostname[256];
	//gethostname(hostname,255);
	//int numFiles = atoi(argv[1]);
	int totalAtMaster;
	total_t1 = MPI_Wtime();
	if (myrank == 0) {
		 // masterWithoutLoadCalc(numFiles);
		 totalAtMaster = masterWithOutLoadCalc(numFiles);
	} else {
		 slave(l1Folder, l2Folder);
	}

    double total_t2 = MPI_Wtime();
    
    if(myrank == 0)
	   cout<<endl<<"P"<<myrank<<" Total # @Root "<<totalAtMaster<<" Total time "<<(total_t2 - total_t1)<<endl<<endl;

	MPI_Finalize();       /* cleanup MPI */
	return 0;
}

int processRefinement(char *fileLayer1, char *fileLayer2)
{   
    	FileReader fileReader;
    	list<Geometry*> *geomsLayer1 = fileReader.readWKTFile(fileLayer1, NULL);
    	
    	if(geomsLayer1 == NULL) 
    	{
    	   //cerr<<"Error: Empty Layer 1 file "<<fileLayer1<<endl;
    	   return 0;
    	}
	    
	    //cerr<<"Layer1 File #"<<fileLayer1<<", geoms, "<<geomsLayer1->size()<<endl;
    	
   		list<Geometry*> *geomsLayer2 = fileReader.readWKTFile(fileLayer2, NULL);
	    
	    if(geomsLayer2 == NULL)
	    {
    	   //cerr<<"Error: Empty Layer 2 file "<<fileLayer2<<endl; 
    	   return 0;
        }
        
	    Index index;
        index.createRTree(geomsLayer1);

    	//cerr<<"P"<<conf.rank<<" "<<hostname<<", Layer2 File #"<<(fileNum)<<", geoms, "<<geomsLayer2->size()<<endl;	
    	   
	    map<Geometry*, vector<void *> >*joinResult = index.query(geomsLayer2);
    
        Join joinObj;
    
        list<pair<Geometry*, Geometry*> >*pairs = joinObj.join(joinResult);
        
        //cout<<"Output Pairs Task "<<fileLayer1<<": "<<pairs->size()<<endl;
        
        return pairs->size();
        
}

void slave(string l1Folder, string l2Folder)
{
  //string l1Input_folder = "/home/satish/mpigis/indexed_data/cemetery/64Parts/";
  //string l1Input_folder = "/home/satish/mpigis/indexed_data/roads25GB/1024Parts/";
  //string l2Input_folder = "/home/satish/mpigis/indexed_data/cemetery/64Parts/";
  //string l2Input_folder = "/home/satish/mpigis/indexed_data/cemetery/64Parts/";
  //string l2Input_folder = "/home/satish/mpigis/indexed_data/sports/64Parts/";
  
  //string l2Input_folder = "/home/satish/mpigis/indexed_data/sports/1024Parts/";
  
  int                 work;
  MPI_Status          status;

  for (;;) {
		MPI_Recv(&work, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    /*
	 * Check the tag of the received message.
	 */
		if (status.MPI_TAG == DIETAG) {
			return;
		}
        string strLayer1 = l1Folder + to_string(work);
        char *layer1 = const_cast<char*>(strLayer1.c_str());
        
        string strLayer2 = l2Folder + to_string(work);
        char *layer2 = const_cast<char*>(strLayer2.c_str());
        
        int joinResults = processRefinement(layer1, layer2);
        
		MPI_Send(&joinResults, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
}

int masterWithOutLoadCalc(int numFiles)
{
   int	nProcs, rank, work;
	int       result;
	
	MPI_Status     status;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcs);          /* #processes in application */
	
	int* totalWorkDoneArr = (int*)malloc(sizeof(int) * nProcs);
	
	for(int i = 0; i<nProcs; i++)
	{
	  totalWorkDoneArr[i] = 0;
	}
	
	//map<long, int, std::greater<long> >* loadByCell = LoadBalancer::calculateLoad();
// 	
// 	vector<int> fileNames;
// 	
// 	for ( const auto &myPair : *loadByCell ) {
//         cout << myPair.second << " -> " << myPair.first << endl;
//         fileNames.push_back(myPair.second);
//     }
/*
* Seed the slaves.
*/
    int fileIndex = 0;
    
	for (rank = 1; rank < nProcs; ++rank) {

		if(fileIndex < numFiles) {
    		work = fileIndex; /* get_next_work_request */;
	    	MPI_Send(&work, 1, MPI_INT, rank, WORKTAG, MPI_COMM_WORLD);
	    	
	    	fileIndex++;
	    }
	}

/*
* Receive a result from any slave and dispatch a new work
* request work requests have been exhausted.
*/
	while (fileIndex < numFiles) {
		MPI_Recv(&result, 1, MPI_INT,  MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);   
		int sender = status.MPI_SOURCE;
        totalWorkDoneArr[sender] += result;
        //cout<<"Result "<<result<<endl;
        
		work = fileIndex;
		MPI_Send(&work, 1, MPI_INT, status.MPI_SOURCE, WORKTAG, MPI_COMM_WORLD);
		
		fileIndex++;
	}
/*
* Receive results for outstanding work requests.
*/
	for (rank = 1; rank < nProcs; ++rank) {
		MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		int sender = status.MPI_SOURCE;
        totalWorkDoneArr[sender] += result;
	}
/*
* Tell all the slaves to exit.
*/
	for (rank = 1; rank < nProcs; ++rank) {
		MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
		//cout<<"Output found by P[" <<rank<<"]: "<<totalWorkDoneArr[rank]<<endl;
	}
	
	// calc total join pairs
	int totalJoinPairsAtRoot = 0;
	for(int id = 1; id < nProcs; id++)
	{
	   totalJoinPairsAtRoot += totalWorkDoneArr[id];
	}
	return totalJoinPairsAtRoot;
}

