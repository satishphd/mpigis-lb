MPI-GIS software uses Message Passing Interface and MPI-IO to read and distribute the geospatial intersection computation among MPI processes running on multiple compute nodes in a cluster for geospatial join and overlay on a cluster of compute nodes.

This code is based on the following two papers:

1) Satish Puri, Anmol Paudel, Sushil K. Prasad:
MPI-Vector-IO: Parallel I/O and Partitioning for Geospatial Vector Data. ICPP 2018: 13:1-13:11

2) Satish Puri, Sushil K. Prasad:
A Parallel Algorithm for Clipping Polygons with Improved Bounds and a Distributed Overlay Processing System Using MPI. CCGRID 2015: 576-585

3) Puri, S. (2019). SpatialMPI: Message Passing Interface for GIS Applications. The Geographic Information Science & Technology Body of Knowledge (2nd Quarter 2019 Edition), John P. Wilson (Ed.). DOI: 10.22224/gistbok/2019.2.6

Example to show how to use: test_mpi-vector-io.cpp

Parser Interface (Parser.h)
	virtual list<Geometry*>* parse(const FileSplits &split) = 0;

FileSplits is defined as a list of "string". It represents a chunk of file, that an MPI process gets after file partioning using MPI-IO.

Example: WKTParser class implements Parser Interface to return a list of Geometry instances given a list of string representation of co-ordinates
