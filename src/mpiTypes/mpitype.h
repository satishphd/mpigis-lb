#ifndef __MPITYPE_H_INCLUDED__
#define __MPITYPE_H_INCLUDED__


#include<mpi.h>
#include<geos/geom/Geometry.h>
#include <stddef.h>
#include <string>

using namespace std;

using namespace geos::geom;

typedef struct ExtraInfo {
  int id;
  string vertexStr;
} ExtraInfo;

typedef struct GeomInfo {
  int vertexStrLength;
  Envelope *env;
  //int cellId;
}GeomInfo;

typedef struct CollectiveInfo {
  int layer1BufLen;
  int layer2BufLen;
}CollectiveInfo;

typedef struct AllObjCollectiveInfo {
  int derivedLayer1BufLen;
  long realLayer1BufLen;
  int layer2BufLen;
}AllObjCollectiveInfo;

typedef struct bbox {
        double x1;
        double x2;
        double y1;
        double y2;
        int id;
        int cellId;
        int vertStrLen;
}bbox;

typedef struct roadbox {
        double x1;
        double x2;
        double y1;
        double y2;
        
        long edgeId;
        long startNodeId;
        long endNodeId;
        long wayId;
        
        //char tag[30];
        
        int cellId;
}roadbox;

typedef struct RoadInfo {
  long edgeId;
  Envelope *env;
}RoadInfo;


class SpatialTypes
{
   //const int PACKING_LEN = 100;
   //MPI_Datatype mbrtype;
  
   //MPI_Op boxUnion;
  
   //void createOperatorType();
   
   static void mbrUnion(Envelope *in, Envelope *inout, int *len, MPI_Datatype *type);
   
   static void mbrUnionS(Envelope *in, Envelope *inout, int *len, MPI_Datatype *type);
   
   public:
  
   SpatialTypes() {}
  
   MPI_Datatype create_MPI_CHARS(int packing_length);
  
   Envelope reduceByUnion(Envelope *env);
   
   Envelope reduceByUnionTest(Envelope *env); 
   
   MPI_Datatype createBoxType();
   
   MPI_Datatype createRoadBoxType();

   MPI_Datatype createCollectiveInfoType();
   
   MPI_Datatype createAllObjeCollectiveInfoType() ;
   //MPI_Datatype createGeomInfoType();
  
};
#endif