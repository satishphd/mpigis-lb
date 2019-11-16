#ifndef __MPI_LARGEFILEPARTITIONER_H_INCLUDED__
#define __MPI_LARGEFILEPARTITIONER_H_INCLUDED__

#include "FilePartitioner.h"
#include "../mpiTypes/mpitype.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include<limits>

class MPI_LargeFile_Partitioner : public FilePartitioner
{
    static const MPI_Offset overlap=1000*2000;
    MPI_File mpi_layer1;
    MPI_File mpi_layer2;
    
    MPI_Datatype MPI_CHARS; 
    int PACKING_LEN = 100;
    
    int rank;
    int MPI_Processes;
               	
	public:
	MPI_LargeFile_Partitioner()
	{
	    SpatialTypes types;
 	    MPI_CHARS = types.create_MPI_CHARS(PACKING_LEN);
	}
	
	MPI_File initializeLayer(Config &args);
	FileSplits* partitionLayer(MPI_File mpi_layer);
	
	//int initMPI(int argc, char **argv);
	
    int initialize(Config &args);
    pair<FileSplits*,FileSplits*> partition(); 
    
    void finalize();
    ~MPI_LargeFile_Partitioner()
    {
    }
    
};
#endif 