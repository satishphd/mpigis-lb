#ifndef __READBINARYMBRFILE_H_INCLUDED__
#define __READBINARYMBRFILE_H_INCLUDED__

#include <mpi.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <geos/geom/Geometry.h>
#include "../parser/Parser.h"
#include "../parser/road_network_parser.h"
#include "../geom_util/seqOSMJoin.h"

//#define FILESIZE      (1048576*1024)  // 1 MB * 1024 = 1 GB

//#define FLOATS_PER_BLK  (1048576*64)  // 64 MB

//#define FLOATS_PER_BLK  (1048576*128)  // 128 MB
#define FLOATS_PER_BLK (100*1024)  

using namespace std;

using namespace geos::geom;

/* noncontiguous access with a single collective I/O function */

class ReadBinaryMBRFile
{
   public:
   list<Geometry*>* parseMBRs(float *buf, MPI_Offset nfloats);
   
   //list<Geometry*>* readMBRs(char *road_networkPath);
    
   list<Envelope*>* readMBRs(char *road_networkPath);
   list<Envelope*>* readMBRs_Contiguous(char *road_networkPath);
   
   list<Envelope*>* parseEnvs(float *buf, MPI_Offset nfloats);
   
};

#endif
