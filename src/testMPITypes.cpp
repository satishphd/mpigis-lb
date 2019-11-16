#include<iostream>
#include "mpiTypes/mpitype.h"


/*
void mbrUnion(Envelope *in, Envelope *inout, int *len, MPI_Datatype *type) 
{    
        double x1, x2, y1, y2;
     
        if(in->getMinX() < inout->getMinX())
            x1 = in->getMinX();
        else
            x1 = inout->getMinX();

		if(in->getMaxX() > inout->getMaxX())
            x2 = in->getMaxX();
        else
            x2 = inout->getMaxX();

 		if(in->getMinY() < inout->getMinY())
            y1 = in->getMinY();
        else
            y1 = inout->getMinY();

		if(in->getMaxY() > inout->getMaxY())
            y2 = in->getMaxY();
        else
            y2 = inout->getMaxY();
        
        *inout = Envelope(x1, x2, y1, y2);
            
} */

int main(int argc, char **argv)
{
  int rank, MPI_Processes;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &MPI_Processes);

  Envelope *env;
  
  //printf("%d %d \n", rank, MPI_Processes);
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
  printf("%d, (%f %f), (%f %f) \n", rank, env->getMinX(), env->getMaxX(), env->getMinY(), env->getMaxY());
  
  SpatialTypes types;
  
  Envelope recv = types.reduceByUnion(env);
  
  /*
  MPI_Op boxUnion;
  MPI_Datatype mbrtype;
  
  MPI_Type_contiguous(4, MPI_DOUBLE, &mbrtype);
  MPI_Type_commit(&mbrtype);
  
  MPI_Op_create(mbrUnion, 0, &boxUnion);
  
  MPI_Reduce(env, &recv, 1, mbrtype, boxUnion, 0, MPI_COMM_WORLD);
  */
  //MPI_Reduce(env, &recv, 1, MPI);
  if(rank == 0)
     printf("%d, (%f %f), (%f %f) \n",0, recv.getMinX(), recv.getMaxX(), recv.getMinY(), recv.getMaxY());
  
  MPI_Finalize();
     
  return 0;
}