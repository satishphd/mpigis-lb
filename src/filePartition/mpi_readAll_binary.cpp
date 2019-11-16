/* noncontiguous access with a single collective I/O function */
#include "mpi.h"
#include <stdlib.h>
#include <iostream>
#include <vector>

//#define FILESIZE      (1048576*1024)  // 1 MB * 1024 = 1 GB
#define FLOATS_PER_BLK  (1048576*64)  // 64 MB
//#define FLOATS_PER_BLK  (1024*1024)  // 1 MB

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
   cout<<"Rank "<<rank<<":"<<numLines<<endl;
}

/* The buf contains x1, y1, x2, y2 */
vector<Envelope*>*(float *buf, MPI_Offset nfloats)
{
   vector<Envelope*>* envVec = new vector<Envelope*>();
   MPI_Offset counter;
   
   for(counter = 0; count< nfloats; counter = counter+4) {
     Envelope *env = new Envelope(buf[counter + 0], buf[counter + 2], buf[counter + 1], buf[counter + 3]);
     envVec->push_back(env);
   }
   
   return envVec;
} 

vector<Envelope*>* readMBRs(char *road_networkPath)
{
  MPI_File fh;
   
  MPI_File_open(MPI_COMM_WORLD, road_networkPath, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
 
  MPI_Datatype filetype;
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  //filesize = 1048576*60; // 64 MB
  
  MPI_Offset bufsize = filesize/nprocs;
  
  if(bufsize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  float *buf = (float *) malloc(bufsize);
   
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 

  MPI_Offset nfloats = bufsize/sizeof(float);
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"floats per process "<<nfloats<<endl;
  }
  
// int MPI_Type_vector(int count,
//                    int blocklength,
//                    int stride, 
//                    MPI_Datatype oldtype,
//                    MPI_Datatype *newtype);

  MPI_Type_vector(nfloats/FLOATS_PER_BLK, FLOATS_PER_BLK, 
                  FLOATS_PER_BLK*nprocs, MPI_FLOAT, &filetype);
                  
  MPI_Type_commit(&filetype);
  MPI_File_set_view(fh, FLOATS_PER_BLK*sizeof(float)*rank, MPI_FLOAT, 
	      filetype, "native", MPI_INFO_NULL);

  MPI_Status status;
  
  MPI_File_read_all(fh, buf, nfloats, MPI_FLOAT, &status);
  
  // After receiving the message, check the status to determine
    // how many numbers were actually received
  int number;
  MPI_Get_count(&status, MPI_FLOAT, &number);

  // Print off the amount of numbers, and also print additional
  // information in the status object
   printf("%d read %d FLOAT \n",rank, number);
  
  parseMBRs(buf, nfloats);
  
  MPI_File_close(&fh);
  
//  buf[nbytes] = '\0';
  //countNumLines(buf, nbytes);

  MPI_Type_free(&filetype);
  
  free(buf);
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
//  char allobjPath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/allobjects/all_objects"; 
//	char road_networkPath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/road_networkdirectory/road_network"; 
  char road_networkPath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/sample_road_network/road_networkMBRsOnly.bin"; 
  MPI_File fh;
   
  MPI_File_open(MPI_COMM_WORLD, road_networkPath, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
 
  MPI_Datatype filetype;
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  //filesize = 1048576*60; // 64 MB
  
  MPI_Offset bufsize = filesize/nprocs;
  
  if(bufsize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  float *buf = (float *) malloc(bufsize);
   
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 

  MPI_Offset nfloats = bufsize/sizeof(float);
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"floats per process "<<nfloats<<endl;
  }
  
// int MPI_Type_vector(int count,
//                    int blocklength,
//                    int stride, 
//                    MPI_Datatype oldtype,
//                    MPI_Datatype *newtype);

  MPI_Type_vector(nfloats/FLOATS_PER_BLK, FLOATS_PER_BLK, 
                  FLOATS_PER_BLK*nprocs, MPI_FLOAT, &filetype);
                  
  MPI_Type_commit(&filetype);
  MPI_File_set_view(fh, FLOATS_PER_BLK*sizeof(float)*rank, MPI_FLOAT, 
	      filetype, "native", MPI_INFO_NULL);

  MPI_Status status;
  
  MPI_File_read_all(fh, buf, nfloats, MPI_FLOAT, &status);
  
  // After receiving the message, check the status to determine
    // how many numbers were actually received
  int number;
  MPI_Get_count(&status, MPI_FLOAT, &number);

  // Print off the amount of numbers, and also print additional
  // information in the status object
   printf("%d read %d FLOAT \n",rank, number);
  
  MPI_File_close(&fh);
  
//  buf[nbytes] = '\0';
  //countNumLines(buf, nbytes);

  MPI_Type_free(&filetype);
  free(buf);
  
  total_t2 = MPI_Wtime();
  
  long longNumber = (long)number;
  
  long totalLines;
  //MPI_Barrier(MPI_COMM_WORLD);
  MPI_Reduce(&longNumber, &totalLines, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
  
  if(rank == 0) {
      cout<<"Total # of lines "<<totalLines<<endl;
	  cout<<"Time = "<<(total_t2 - total_t1)<<endl;
  }
  
  MPI_Finalize();
  return 0; 
}

