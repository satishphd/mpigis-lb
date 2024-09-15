#include<iostream>
#include<mpi.h>
#include<stddef.h>
#include "newType.h"

// mpicxx -o type PointType.cpp
// mpirun -np 3 ./type

struct Rect 
{
        double x1;
        double x2;
        double y1;
        double y2;
        int id;       
};

struct Point2D 
{
        double x1;
        double y1;
        int id;       
};

/*
int MPI_Type_create_struct(int count,
                          int array_of_blocklengths[],
                          MPI_Aint array_of_displacements[],
                          MPI_Datatype array_of_types[],
                          MPI_Datatype *newtype)
*/
using namespace std;

void createPointType(MPI_Datatype *newType)
{
    const int nitems = 3;
    int          blocklengths[3] = {1,1,1};
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
  
    MPI_Aint     offsets[3];
    
    offsets[0] = offsetof(Point2D, x1);
    offsets[1] = offsetof(Point2D, y1);
    offsets[2] = offsetof(Point2D, id);
    
    MPI_Type_create_struct( nitems, blocklengths, offsets, types, newType );
    
    MPI_Type_commit(newType);
}

void createRectType( MPI_Datatype *newType )
{
    const int nitems=5;
    int blocklengths[5] = {1,1,1,1,1};
    MPI_Datatype types[5] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
  
    MPI_Aint     offsets[5];
    
    offsets[0] = offsetof(Rect, x1);
    offsets[1] = offsetof(Rect, x2);
    offsets[2] = offsetof(Rect, y1);
    offsets[3] = offsetof(Rect, y2);
    offsets[4] = offsetof(Rect, id);
    
    MPI_Type_create_struct( nitems, blocklengths, offsets, types, newType );

    MPI_Type_commit(newType);

}

int testMPISpatialCommunication( MPI_Datatype pointType, MPI_Datatype rectType )
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
    in_rect.id = 101;
    
    in_point.x1 = -12.5;
    in_point.y1 = -14.5;
    in_point.id = 200;
  }
  
  MPI_Bcast(&in_point, 1, pointType, 0, MPI_COMM_WORLD);
  MPI_Bcast(&in_rect, 1, rectType, 0, MPI_COMM_WORLD);

  if(rank != 0) {
    cout<<"Point "<<in_point.y1<<endl;  // -14.5;
    cout<<"Rect "<<in_rect.y2<<endl;    // 5.0;
  }
    
  return 0;
}

int main(int argc, char **argv)
{
  int rank, MPI_Processes;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &MPI_Processes);
  
  MPI_Datatype pointType;
  createPointType( &pointType ); 
  
  MPI_Datatype rectType;
  createRectType( &rectType ); 
  
  testMPISpatialCommunication( pointType, rectType );
  
  MPI_Finalize();
     
  return 0;
}
