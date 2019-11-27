#include<iostream>
#include "spatialMPI.h"

void testMPISpatialReduction()
{
   // Envelope (double x1, double x2, double y1, double y2)
   Envelope *env;
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   
   switch(rank) 
   {
     case 0: env = new Envelope(0.0, 5.0, 1.0, 3.0); 
      break;

     case 1: env = new Envelope(2.0, 7.0, 0.0, 12.0); 
      break;
    
     case 2: env = new Envelope(1.0, 2.0, -2.0, 2.0); 
      break;

     case 3: env = new Envelope(0.0, 1.0, 3.0, 7.0); 
      break;
      
     default: env = new Envelope(0.0, 2.0, 0.0, 2.0); 
   }  
   
    Envelope recv;
    
    MPI_Allreduce(env, &recv, 1, MPI_RECT, MPI_UNION, MPI_COMM_WORLD);
    
    printf("%d, (%f %f), (%f %f) \n", rank, recv.getMinX(), recv.getMaxX(), recv.getMinY(), recv.getMaxY());
}

int testMPISpatialCommunication()
{
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  Rect in_rect;  
  Point2D in_point;
  
  if(rank == 0) {
  	in_rect.x1 = 1.0;
    in_rect.x2 = 2.0;
    in_rect.y1 = 3.0;
    in_rect.y2 = 5.0;
    
    in_point.x1 = -12.5;
    in_point.y1 = -14.5;
  }
  
  MPI_Bcast(&in_point, 1, MPI_POINT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&in_rect, 1, MPI_RECT, 0, MPI_COMM_WORLD);

  if(rank != 0) {
    cout<<"Rect "<<in_rect.y2<<endl;
    cout<<"Point "<<in_point.y1<<endl;
  }
    
  return 0;
}

int main(int argc, char **argv)
{
  int rank, MPI_Processes;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &MPI_Processes);
  
  SpatialMPI mpiEnv;
  mpiEnv.initSpatialMPI();
  
  testMPISpatialCommunication();
  
  testMPISpatialReduction();
  
  MPI_Finalize();
     
  return 0;
}


