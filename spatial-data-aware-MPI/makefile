CC = mpicxx
LIBB = -L/usr/local/Cellar/geos/3.5.0/lib -lgeos
#LIBB = -L/sw/geosoft/geos/lib -lgeos
#CFLAGS = -Wall -c -std=c++0x -I/sw/geosoft/geos/include
#LIBRA = ../lib/libshp.a
DEBUG = -g
CFLAGS = -Wall -c -I/usr/local/Cellar/geos/3.5.0/include
LFLAGS = -Wall

all: spatialMPI

spatialMPI: spatialMPI.o newtype.o testSpatialTypes.o
	$(CC) -o test spatialMPI.o newtype.o testSpatialTypes.o $(LIBB)

spatialMPI.o: spatialMPI.cpp
	$(CC) $(CFLAGS) spatialMPI.cpp

newtype.o: newtype.cpp
	$(CC) $(CFLAGS)	newtype.cpp	
	
testSpatialTypes.o: testSpatialTypes.cpp	
	$(CC) $(CFLAGS)	testSpatialTypes.cpp
