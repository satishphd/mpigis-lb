#ifndef __READBINARY_MBR_STRUCT_H_INCLUDED__
#define __READBINARY_MBR_STRUCT_H_INCLUDED__

#include <mpi.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include<tuple>
#include <geos/geom/Geometry.h>

#include "../mpiTypes/mpitype.h"

#include "../parser/Parser.h"
//#include "../parser/road_network_parser.h"
#include "../geom_util/seqOSMJoin.h"

typedef struct MBR
{
   double minx;
   double miny;
   double maxx;
   double maxy;
}MBR;

typedef struct IntMBR
{
   int minx;
   int miny;
   int maxx;
   int maxy;
}IntMBR;

using namespace std;

using namespace geos::geom;

/* noncontiguous access with a single collective I/O function */



class MPIReadBinMBRStruct
{
   const int MBR_PER_BLK = 2;
   
   list<Envelope*>* parseEnvs(MBR *mbrs, MPI_Offset nmbrs);
  
   list<Envelope*>* parseIntEnvs(IntMBR *mbrs, MPI_Offset nmbrs);
   
   public:
   
   list<Envelope*>* readMBRs_Contiguous(char *mbrPath);
   
   list<Envelope*>* readIntMBRs_Contiguous(char *mbrPath);

   list<Envelope*>* readMBRs_NonContig(char *mbrPath);
   
   MPI_Datatype createMBRType();
   MPI_Datatype createIntMBRType();
  
};

#endif
