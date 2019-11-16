#ifndef __READBINARY_POINT_H_INCLUDED__
#define __READBINARY_POINT_H_INCLUDED__

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

typedef struct pointNode
{
   double longi;
   double lati;
   long nodeId;
   char tag[32];
}pointNode;

//#define FILESIZE      (1048576*1024)  // 1 MB * 1024 = 1 GB
//#define POINT_NODES_PER_BLK  (1048576*64)  // 64 MB
//#define FLOATS_PER_BLK  (1048576*128)  // 128 MB

//#define POINT_NODES_PER_BLK 1024  // 1 MB

using namespace std;

using namespace geos::geom;

/* noncontiguous access with a single collective I/O function */



class ReadBinaryPointFile
{
   const int POINT_NODES_PER_BLK = 1024;
   
   public:
   //list<Geometry*>* parseMBRs(float *buf, MPI_Offset nfloats);
   
   //list<Geometry*>* readMBRs(char *road_networkPath);
    
   //list<Envelope*>* readMBRs(char *road_networkPath);
   pair<int, pointNode*>* readPoints_Contiguous(char *pointPath);
   
   pair<int, pointNode*>* readPoints_NonContig(char *pointPath);
   
   MPI_Datatype createPtNodeType();
   //list<Envelope*>* parseEnvs(float *buf, MPI_Offset nfloats);
   
};

#endif
