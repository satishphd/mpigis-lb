#ifndef __SPATIALMPI_H_INCLUDED__
#define __SPATIALMPI_H_INCLUDED__

#include<mpi.h>
#include<stddef.h>
#include<geos/geom/Geometry.h>

#include "newtype.h"

using namespace std;
using namespace geos::geom;
 
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

struct Line 
{
        double x1;
        double x2;
        double y1;
        double y2;
        int id;       
};

class SpatialMPI
{
  private:
 
   void initDataTypes();
   void initOperators();
   
   MPI_Datatype createRectType();
   MPI_Datatype createLineType();
   MPI_Datatype createPointType();
  
  
  public:
  
  // static void mbrUnion(Envelope *in, Envelope *inout, int *len, MPI_Datatype *type); 
   static void mbrUnion(void *in, void *inout, int *len, MPI_Datatype *type); 
   void initSpatialMPI();
};

#endif