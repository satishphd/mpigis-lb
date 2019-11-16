#include "loadbalance/staticMPILoadCalculator.h"

// /home/satish/mpigis/indexed_data/layer1
// /home/satish/mpigis/indexed_data/layer2

// mpirun -np 2 ./adlb 8 /home/satish/mpigis/indexed_data/layer1 /home/satish/mpigis/indexed_data/layer2
// e.g. mpirun -np 4 ./adlb 64 /home/satish/mpigis/indexed_data/cemetery/64Parts/ /home/satish/mpigis/indexed_data/sports/64Parts/
// first argument is number of files. This value is ignored currently: the actual number of files is read from the folder.


//int main(int argc, char **argv)
map<long, int, std::greater<long> > * StaticMPILoadCalculator :: calculateLoad(int numFiles, string l1Mbr_folder, string l2Mbr_folder)
{	
//    Config conf(argc, argv); 
    
//    conf.initMPI(argc, argv);
    int numProcesses, rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double total_t1;
	
	char hostname[256];
	gethostname(hostname,255);
	
	total_t1 = MPI_Wtime();
	
   //  int rank = conf.rank;
//     int numProcesses = conf.numProcesses;
//     int numFiles = conf.numPartitions;
    cout<<"#indexed files "<<numFiles<<", #processes "<<numProcesses<<endl;
    
    map<int, vector<Envelope*>* > *l1CellMBRs = new map<int, vector<Envelope*>* >();
    map<int, vector<Envelope*>* > *l2CellMBRs = new map<int, vector<Envelope*>* >();
    
   // string l1Mbr_folder = "/home/satish/mpigis/indexed_data/roads25GB/index/2048Parts/";
   // string l2Mbr_folder = "/home/satish/mpigis/indexed_data/sports/index/2048Parts/";
    
    for(int fileNum = rank; fileNum < numFiles; fileNum = fileNum + numProcesses) {
        
    	string l1File = l1Mbr_folder + to_string(fileNum);
    	string l2File = l2Mbr_folder + to_string(fileNum);
        
        vector<Envelope*> *l1Envs = new vector<Envelope*>();
     
        vector<Envelope*> *l2Envs = new vector<Envelope*>();
        
        int l1Count = readMBRFile(l1File, l1Envs);
        
        int l2Count = readMBRFile(l2File, l2Envs);
   
        if(l1Count > 0 && l2Count > 0)
        {
           l1CellMBRs->insert(pair<int, vector<Envelope*>* >(fileNum, l1Envs));   
           l2CellMBRs->insert(pair<int, vector<Envelope*>* >(fileNum, l2Envs));   
           //cout<<l1Count<<" "<<l2Count<<endl;
        }
     }
     
    map<long, int, std::greater<long> > *loadByCellMap = createReducers(numFiles, l1CellMBRs, l2CellMBRs);

    double total_t2 = MPI_Wtime();
  
//    if(conf.rank == 0)
//	cout<<endl<<"P"<<conf.rank<<" "<<"Total time "<<(total_t2 - total_t1)<<endl<<endl;
  
  //  MPI_Finalize();
    
    return loadByCellMap;
}
