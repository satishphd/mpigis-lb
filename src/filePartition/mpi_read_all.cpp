/* noncontiguous access with a single collective I/O function */
#include "mpi.h"
#include <stdlib.h>
#include <iostream>

//#define FILESIZE      (1048576*1024)  // 1 MB * 1024 = 1 GB
#define BYTES_PER_BLK  (1048576*64)  // 64 MB

using namespace std;

int countNumLines(char *stream, MPI_Offset len)
{
   MPI_Offset byteCounter;
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   long numLines = 0;
   //for(int i = 0; i< 1000; i++)
      //printf("%c",stream[i]);
   //cout<<endl;   
   for(byteCounter = 0; byteCounter < len; byteCounter++) {
      if( stream[byteCounter] == '\n')
         numLines++;
   }
   //cout<<"Rank "<<rank<<":"<<numLines<<endl;
   long totalLines;

  MPI_Reduce(&numLines, &totalLines, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
 
  if(rank == 0)
    cout<<"Total Lines "<<totalLines<<endl;
}

int main(int argc, char **argv)
{
  int rank, nprocs;
  
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  double total_t1, total_t2;
  
  total_t1 = MPI_Wtime();
  
//  char lakeFilePath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/lakes/new_lakes.txt";
//  char road25GBFilePath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/roads25GB/roads"; 
  char allobjPath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/allobjects/all_objects"; 
//	char road_networkPath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/road_networkdirectory/road_network"; 
  
  MPI_File fh;
   
  MPI_File_open(MPI_COMM_WORLD, allobjPath, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
 
  MPI_Datatype filetype;
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  MPI_Offset bufsize = filesize/nprocs;
  
  if(bufsize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  char *buf = (char *) malloc((bufsize+1) * sizeof(char));
   
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 

  MPI_Offset nbytes = bufsize/sizeof(char);
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"bytes per process "<<nbytes<<endl;
  }
  
// int MPI_Type_vector(int count,
//                    int blocklength,
//                    int stride, 
//                    MPI_Datatype oldtype,
//                    MPI_Datatype *newtype);

  MPI_Type_vector(nbytes/BYTES_PER_BLK, BYTES_PER_BLK, 
                  BYTES_PER_BLK*nprocs, MPI_CHAR, &filetype);
                  
  MPI_Type_commit(&filetype);
  MPI_File_set_view(fh, BYTES_PER_BLK*sizeof(char)*rank, MPI_CHAR, 
	      filetype, "native", MPI_INFO_NULL);

  MPI_Status status;
  
  int error_code = MPI_File_read_all(fh, buf, nbytes, MPI_CHAR, &status);
  
  if(MPI_SUCCESS != error_code) {
     int len;
     char error[50];
     MPI_Error_string(error_code, error, &len);
     //cout<<"Error code "<<error<<endl;
     printf("%s \n", error);
  }
  
  // After receiving the message, check the status to determine
    // how many numbers were actually received
  int number;
  MPI_Get_count(&status, MPI_CHAR, &number);

  // Print off the amount of numbers, and also print additional
  // information in the status object
  // printf("%d read %d bytes \n",rank, number);
  long totalBytes;
  long longNumber = (long)number;
  MPI_Reduce(&longNumber, &totalBytes, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
  
  if(rank == 0)
    cout<<"Total bytes "<<totalBytes<<endl;
    
  MPI_File_close(&fh);
  
  buf[nbytes] = '\0';
  countNumLines(buf, nbytes);

  MPI_Type_free(&filetype);
  free(buf);
  
  total_t2 = MPI_Wtime();
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  if(rank == 0)
	  cout<<"Time = "<<(total_t2 - total_t1)<<endl;
  
  MPI_Finalize();
  return 0; 
}

