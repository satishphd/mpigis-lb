#include "filePartition/config.h"

vector<string>* Config :: getFilesFromDir(char *directory)
{
   vector<string> *files = new vector<string>();
   
   DIR *dir;
   struct dirent *ent;

   if ((dir = opendir (directory)) != NULL) 
   {
     /* print all the files and directories within directory */
     while ((ent = readdir (dir)) != NULL) 
     {
       //printf ("%s\n", ent->d_name);
       string path = string(directory) + "/" + string(ent->d_name);
       files->push_back(path);
     }
     closedir (dir);
   } 
   else
   {
     /* could not open directory */
     return NULL;
   }
   return files;
}

vector<string>* Config :: getLayer1() 
{
      return layer1;
}
    
vector<string>* Config :: getLayer2() 
{
      return layer2;
}

// Config &args
int Config :: initMPI(int argc, char **argv)
{

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
        
    //MPI_Op mbrUnion = SpatialTypes :: createOperatorType();
    cout<<argc<<endl;
    if (argc < 3) {
        if (rank == 0) cout<<"Usage: mpirun -np #processes ./mpi_program #partitions directory1 directory2 log_file"<<endl;
        MPI_Finalize();
        exit(1);
    }
    return 0;
}
