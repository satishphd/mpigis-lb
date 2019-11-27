#include "filePartition/FilePartitioner.h"
#include "filePartition/MPI_File_Partitioner.h"
#include "filePartition/FileSplits.h"
#include "parser/Parser.h"
#include "parser/WKTParser.h"
#include "filePartition/config.h"
#include "mpiTypes/mpitype.h"
#include "geom_util/util.h"

#include <geos/geom/Geometry.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <list>


/*
 * This example is extracted from a spatial join program using MPI-GIS-II with two input data directories and #grid cells for
 * spatial partitioning.
 *
 * The program has dependency on GEOS opensource library
 * mpicxx -std=c++0x -o mpi-vector-io test_mpi-vector-io.cpp mpigis.a -I/usr/local/include -L/usr/local/lib -lgeos
 *
 * Command line arguments are number of grid cells and two input directories containing data from 
 *                                                                http://spatialhadoop.cs.umn.edu/datasets.html
 * mpirun -np 8 ./mpi-vector-io #grid_cells data_director1 data_directory2
 * file1:    /projects/GeoSpatial/data/SpatialHadoop_DataSets/cemetery
 */
 
int main(int argc, char **argv) 
{	
    Config args(argc, argv); 
    
    args.initMPI(argc, argv);
    
    FilePartitioner *partitioner = new MPI_File_Partitioner();
	
    partitioner->initialize(args);
    
    pair<FileSplits*, FileSplits*> splitPair = partitioner->partition();
    
    Parser *parser = new WKTParser();
    
    list<Geometry*> *layer1Geoms = parser->parse(*splitPair.first);
    cout<<layer1Geoms->size()<<endl;
    
    list<Geometry*> *layer2Geoms = parser->parse(*splitPair.second);
    
    MPI_Finalize();
}
