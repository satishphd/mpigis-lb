#include "loadbalance/SingleMasterLoadCalculator.h"

#include <mpi.h>

#define WORKTAG     1
#define DIETAG     2

//https://www.hpc.cam.ac.uk/using-clusters/compiling-and-development/parallel-programming-mpi-example
int main(int argc, char *argv[])
{
	int         myrank;
	MPI_Init(&argc, &argv);   /* initialize MPI */
	MPI_Comm_rank(MPI_COMM_WORLD,  &myrank);      /* process rank, 0 thru N-1 */

    double total_t1;
	
	//char hostname[256];
	//gethostname(hostname,255);
	
	total_t1 = MPI_Wtime();
	if (myrank == 0) {
		LoadBalancer:: master();
	} else {
		LoadBalancer:: slave();
	}

    double total_t2 = MPI_Wtime();
   
//    if(conf.rank == 0)
	cout<<endl<<"P"<<myrank<<" "<<"Total time "<<(total_t2 - total_t1)<<endl<<endl;

	MPI_Finalize();       /* cleanup MPI */
	return 0;
}

int LoadBalancer :: processRefinement(char *fileLayer1, char *fileLayer2)
{   
    	FileReader fileReader;
    	list<Geometry*> *geomsLayer1 = fileReader.readWKTFile(fileLayer1, NULL);
    	
    	if(geomsLayer1 == NULL) 
    	{
    	   cerr<<"Error: Empty Layer 1 file "<<endl;
    	   return 0;
    	}
	    
	    //cerr<<"Layer1 File #"<<fileLayer1<<", geoms, "<<geomsLayer1->size()<<endl;
    	
   		list<Geometry*> *geomsLayer2 = fileReader.readWKTFile(fileLayer2, NULL);
	    
	    if(geomsLayer2 == NULL)
	    {
    	   cerr<<"Error: Empty Layer 2 file "<<endl; 
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

void LoadBalancer :: slave()
{
  //string l1Input_folder = "/home/satish/mpigis/indexed_data/sports/64Parts/";
  string l1Input_folder = "/home/satish/mpigis/indexed_data/roads25GB/1024Parts/";
  //string l1Input_folder = "/home/satish/mpigis/indexed_data/cemetery/64Parts/";
  //string l2Input_folder = "/home/satish/mpigis/indexed_data/cemetery/64Parts/";
  //string l2Input_folder = "/home/satish/mpigis/indexed_data/sports/64Parts/";
  string l2Input_folder = "/home/satish/mpigis/indexed_data/sports/1024Parts/";
  
  double              result;
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
        string strLayer1 = l1Input_folder + to_string(work);
        char *layer1 = const_cast<char*>(strLayer1.c_str());
        
        string strLayer2 = l2Input_folder + to_string(work);
        char *layer2 = const_cast<char*>(strLayer2.c_str());
        
        int joinResults = processRefinement(layer1, layer2);
        
		MPI_Send(&joinResults, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
}

void LoadBalancer :: master()
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
	
	map<long, int, std::greater<long> >* loadByCell = LoadBalancer::calculateLoad();
	
	vector<int> fileNames;
	
	for ( const auto &myPair : *loadByCell ) {
        cout << myPair.second << " -> " << myPair.first << endl;
        fileNames.push_back(myPair.second);
    }
/*
* Seed the slaves.
*/
    int fileIndex = 0;
    
	for (rank = 1; rank < nProcs; ++rank) {

		if(fileIndex < fileNames.size()) {
    		work = fileNames[fileIndex]; /* get_next_work_request */;
	    	MPI_Send(&work, 1, MPI_INT, rank, WORKTAG, MPI_COMM_WORLD);
	    	
	    	fileIndex++;
	    }
	}

/*
* Receive a result from any slave and dispatch a new work
* request work requests have been exhausted.
*/
	while (fileIndex < fileNames.size()) {
		MPI_Recv(&result, 1, MPI_INT,  MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);   
		int sender = status.MPI_SOURCE;
        totalWorkDoneArr[sender] += result;
        //cout<<"Result "<<result<<endl;
        
		work = fileNames[fileIndex];
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
}

map<long, int, std::greater<long> >* LoadBalancer :: calculateLoad()
{
  double load_t1 = MPI_Wtime();
  //string l1Mbr_folder = "/home/satish/mpigis/indexed_data/cemetery/index/64Parts/";
  //string l1Mbr_folder = "/home/satish/mpigis/indexed_data/sports/index/64Parts/";
  //string l1Mbr_folder = "/home/satish/mpigis/indexed_data/lakes/index/1024Parts/";
  string l1Mbr_folder = "/home/satish/mpigis/indexed_data/roads25GB/index/1024Parts/";
  FileReader mbrReader;
  
  int cellCount = 1024;
  
  map<int, list<Envelope*>* >* L1MbrsByCell = mbrReader.readMBRFiles(l1Mbr_folder, cellCount);
  
  //string l2Mbr_folder = "/home/satish/mpigis/indexed_data/cemetery/index/64Parts/";
  string l2Mbr_folder = "/home/satish/mpigis/indexed_data/sports/index/1024Parts/";
  //string l2Mbr_folder = "/home/satish/mpigis/indexed_data/sports/index/64Parts/";
  
  map<int, list<Envelope*>* >* L2MbrsByCell = mbrReader.readMBRFiles(l2Mbr_folder, cellCount);
  
  map<long, int, std::greater<long> > *loadByCellMap = createReducers(cellCount, L1MbrsByCell, L2MbrsByCell);
 
  double load_t2 = MPI_Wtime();
  
  cout<<endl<<"Load calculation time "<<(load_t2 - load_t1)<<endl<<endl;
  
  return loadByCellMap;
}

long LoadBalancer :: reduce(int cellId, list<Envelope*>* col1, list<Envelope*>* col2)
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

map<long, int, std::greater<long> >* LoadBalancer :: createReducers(int cellCount, map<int, list<Envelope*>* >* geomsByCellMap,
 												 map<int, list<Envelope*>* >* queriesByCellMap)
 {   
     map<long, int, std::greater<long> > *loadMap = new map<long, int, std::greater<long> >();
     
     long totalOutput = 0;
     
     for(int cellId = 0; cellId < cellCount; cellId++) {
        if(geomsByCellMap->find(cellId) != geomsByCellMap->end() && 
        								queriesByCellMap->find(cellId) != queriesByCellMap->end()) {
           int output = reduce(cellId, geomsByCellMap->at(cellId), queriesByCellMap->at(cellId)); 
  
           if(output>0) {
             //cout<< " ["<<cellId <<"] : "<<output<<endl;
             loadMap->insert(pair<long,int>(output, cellId));
           }
  
           totalOutput += output;
        }
     }
     //verify(totalOutput);

     return loadMap;
 }
 
 //int main(int argc, char **argv)
//{ 
  // if(argc<3) 
//   {
//     cout<<"Enter file1 and file2"<<endl;
//     return 1;
//   }
  
  //  /home/satish/mpigis/indexed_data/cemetery/index/64Parts
  //FILE *ofp = fopen(argv[2],"w");
  
//   string mbr_folder = "/home/satish/mpigis/indexed_data/cemetery/index/64Parts/";
//   
//   FileReader mbrReader;
//   
//   int cellCount = 64;
//   
//   map<int, list<Envelope*>* >* L1MbrsByCell = mbrReader.readMBRFiles(mbr_folder, cellCount);
//   
//   map<int, list<Envelope*>* >* L2MbrsByCell = mbrReader.readMBRFiles(mbr_folder, cellCount);
//   
//   createReducers(cellCount, L1MbrsByCell, L2MbrsByCell);
//   return 0;
// }

// int main()
// {
//    LoadBalancer::calculateLoad();
//    return 0;
// }
