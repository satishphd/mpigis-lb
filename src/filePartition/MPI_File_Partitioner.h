#ifndef __MPI_FILEPARTITIONER_H_INCLUDED__
#define __MPI_FILEPARTITIONER_H_INCLUDED__

#include "FilePartitioner.h"

#include <assert.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

class MPI_File_Partitioner : public FilePartitioner
{
    static const MPI_Offset overlap=1000*2000;
    MPI_File mpi_layer1;
    MPI_File mpi_layer2;
    
    int rank;
    int MPI_Processes;
               	
	public:
	MPI_File_Partitioner()
	{

	}
	
	MPI_File initializeLayer(Config &args);
	FileSplits* partitionLayer(MPI_File mpi_layer);
	
	//int initMPI(int argc, char **argv);
	
    int initialize(Config &args);
    pair<FileSplits*,FileSplits*> partition(); 
    
    void finalize();
    ~MPI_File_Partitioner()
    {
    }
    
};
#endif 