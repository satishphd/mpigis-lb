#include "loadbalance/dynamicMPILoadCalculator.h"

map<long, int, std::greater<long> >* DynamicMPILoadCalculator :: calculateLoad(int numFiles, 
															string l1Mbr_folder, string l2Mbr_folder)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,  &rank);  
    
    map<int, long>* loadMapByFileID;
    map<long, int, std::greater<long> >* loadMapByLoadFactor = new map<long, int, std::greater<long> >();
   
    if(rank == 0) {
       loadMapByFileID = master(numFiles);
       for(int fileId = 0; fileId < numFiles; fileId++)
       {
           if( loadMapByFileID->find(fileId) != loadMapByFileID->end())
              loadMapByLoadFactor->insert(pair<long, int>(loadMapByFileID->at(fileId), fileId));
       }
    }
    else {
       slave(numFiles, l1Mbr_folder, l2Mbr_folder);
    }
    
    return loadMapByLoadFactor;
}

/*
int  reduce(int cellId, vector<Envelope*>* col1, vector<Envelope*>* col2)
{
    Index tree;
   
    if(col1 != nullptr && !col1->empty())
	 tree.createRTree(col1);

	map<Envelope*, vector<void *> >* intersectionMap = nullptr;	
	
	if(col2 != nullptr && !col2->empty())
      intersectionMap = tree.query(col2);
	
	Join spatialJoin;

    //list<pair<Envelope*, Envelope*> > *pairs = nullptr;
    int joinPairs = 0;
    if(intersectionMap != nullptr)
    {
        joinPairs = spatialJoin.join(intersectionMap);
        //cout<<joinPairs<<" "; 
    }
    
    return joinPairs;
    
    //cout<< "  "<<pairs->size() <<endl;
}
*/

void DynamicMPILoadCalculator :: slave(int numFiles, string l1Mbr_folder, string l2Mbr_folder)
{
  int                 work;
  MPI_Status          status;
     
  //string l1Mbr_folder = "/home/satish/mpigis/indexed_data/roads25GB/index/2048Parts/";
  //string l2Mbr_folder = "/home/satish/mpigis/indexed_data/sports/index/2048Parts/";

  int output = 0;
  FilterResult filterResult;
  FileReader mbrReader;
  
  for (;;) {
		MPI_Recv(&work, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    /*
	 * Check the tag of the received message.
	 */
		if (status.MPI_TAG == DIETAG) {
			return;
		}
        
        string l1File = l1Mbr_folder + to_string(work);
    	string l2File = l2Mbr_folder + to_string(work);
        
        vector<Envelope*> *l1Envs = new vector<Envelope*>();
     
        vector<Envelope*> *l2Envs = new vector<Envelope*>();
        
        int l1Count = mbrReader.readMBRFile(l1File, l1Envs);
        
        int l2Count = mbrReader.readMBRFile(l2File, l2Envs);
   
        if(l1Count > 0 && l2Count > 0)
        {
            output = reduce(work, l1Envs, l2Envs);
            filterResult.pairs = output;
            filterResult.fileID = work; 
           //cout<<l1Count<<" "<<l2Count<<endl;
        }
        else
        {
            filterResult.pairs = 0;
            filterResult.fileID = work;
        }
        
		MPI_Send(&filterResult, 1, FilterResType, 0, 0, MPI_COMM_WORLD);
  }
}

map<int, long>* DynamicMPILoadCalculator :: master(int numFiles)
{   
	int	nProcs, rank, work;
	
	MPI_Status     status;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcs);          /* #processes in application */
	
	//map<long, int, std::greater<long> >* loadMap = new map<long, int, std::greater<long> >();
	map<int, long>* loadMap = new map<int, long>();
	
	int* totalWorkDoneArr = (int*)malloc(sizeof(int) * nProcs);
	
	for(int i = 0; i<nProcs; i++)
	{
	  totalWorkDoneArr[i] = 0;
	}

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
	    FilterResult result;
		
		MPI_Recv(&result, 1, FilterResType,  MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);   
		int sender = status.MPI_SOURCE;
        totalWorkDoneArr[sender] += result.pairs;

        if(result.pairs > 0)
           loadMap->insert(pair<int, long>(result.fileID, result.pairs));
        //cout<<"Result "<<result<<endl;
        
		work = fileIndex;
		MPI_Send(&work, 1, MPI_INT, status.MPI_SOURCE, WORKTAG, MPI_COMM_WORLD);
		
		fileIndex++;
	}
/*
* Receive results for outstanding work requests.
*/
	for (rank = 1; rank < nProcs; ++rank) {
    	FilterResult result;
		
		MPI_Recv(&result, 1, FilterResType, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		int sender = status.MPI_SOURCE;
        totalWorkDoneArr[sender] += result.pairs;
        
        if(result.pairs > 0)
           loadMap->insert(pair<int, long>(result.fileID, result.pairs));
	}
/*
* Tell all the slaves to exit.
*/
	for (rank = 1; rank < nProcs; ++rank) {
		MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
		//cout<<"Output found by P[" <<rank<<"]: "<<totalWorkDoneArr[rank]<<endl;
	}
	
	return loadMap;
}

//https://www.hpc.cam.ac.uk/using-clusters/compiling-and-development/parallel-programming-mpi-example
/*
int main(int argc, char *argv[])
{
	int         myrank;
	MPI_Init(&argc, &argv);  
	MPI_Comm_rank(MPI_COMM_WORLD,  &myrank);      

    if(argc < 1)
      exit(1);
      
    int numFiles = atoi(argv[1]);
    
    double total_t1;
	
	//char hostname[256];
	//gethostname(hostname,255);
	
	total_t1 = MPI_Wtime();
	if (myrank == 0) {
		 master(numFiles);
	} else {
		 slave();
	}

    double total_t2 = MPI_Wtime();
   
//    if(conf.rank == 0)
	cout<<endl<<"P"<<myrank<<" "<<"Total time "<<(total_t2 - total_t1)<<endl<<endl;

	MPI_Finalize();      
	return 0;
}*/

