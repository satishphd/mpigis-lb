GCC = g++
CC = mpicxx
#CC = mpecc --mpicc='mpicxx' -mpilog
#LIBB = -L/usr/local/Cellar/geos/3.5.0/lib 
#LIBB = -L/sw/geosoft/geos/lib -lgeos
LIBB = -L/usr/local/lib -lgeos
#CFLAGS = -O2 -Wall -c -std=c++0x -I/sw/geosoft/geos/include
CFLAGS = -O0 -Wall -c -std=c++0x -I/usr/local/include
#LIBRA = ../lib/libshp.a ../lib/librtree.a
LIBRA =
DEBUG =
#CFLAGS = -Wall -c -std=c++0x -I/usr/local/Cellar/geos/3.5.0/include
LFLAGS = -Wall

#-DNDEBUG add this to disable assert macro

all: allobj

seq: io

refineLB: MPILoadBalancer.o dynamicMPILoadCalculator.o staticMPILoadCalculator.o fileWriter.o GeomPrintReducer.o pointNodeParser.o fileRead.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o
	$(CC) -O0 $(DEBUG) $(LFLAGS) -o refineLB MPILoadBalancer.o dynamicMPILoadCalculator.o staticMPILoadCalculator.o fileWriter.o pointNodeParser.o GeomPrintReducer.o  fileRead.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o $(LIBB)

MPILoadBalancer.o: MPILoadBalancer.cpp
	$(CC)  $(DEBUG) $(CFLAGS) MPILoadBalancer.cpp

dlbLoadCalc: dynamicMPILoadCalculator.o fileWriter.o GeomPrintReducer.o pointNodeParser.o AllObjectsCommBuffer_DerivedType.o  MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o
	$(CC) -O0 $(DEBUG) $(LFLAGS) -o dlbLoadCalc dynamicMPILoadCalculator.o fileWriter.o pointNodeParser.o GeomPrintReducer.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o $(LIBB)

slbLoadCalc: MPILoadBalancer.o staticMPILoadCalculator.o fileRead.o fileWriter.o GeomPrintReducer.o pointNodeParser.o fileRead.o GeomJoinReducer.o uniformGrid.o  spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o WKTParser.o FileSplits.o config.o 
	$(CC) -O0 $(DEBUG) $(LFLAGS) -o slbLoadCalc MPILoadBalancer.o staticMPILoadCalculator.o fileRead.o fileWriter.o pointNodeParser.o GeomPrintReducer.o GeomJoinReducer.o uniformGrid.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o WKTParser.o FileSplits.o config.o  $(LIBB)

dynamicMPILoadCalculator.o: dynamicMPILoadCalculator.cpp
	$(CC)  $(DEBUG) $(CFLAGS) dynamicMPILoadCalculator.cpp

staticMPILoadCalculator.o: staticMPILoadCalculator.cpp
	$(CC)  $(DEBUG) $(CFLAGS) staticMPILoadCalculator.cpp

lbDynamic: adlb_mpigis_dyn.o filter.o AllObjectsCollectiveAttr.o fileWriter.o GeomPrintReducer.o pointNodeParser.o AllObjectsCommBuffer_DerivedType.o  MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o
	$(CC) -O0 $(DEBUG) $(LFLAGS) -o lbDynamic adlb_mpigis_dyn.o filter.o fileWriter.o pointNodeParser.o GeomPrintReducer.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o $(LIBB)

lbStatic: AllObjectsCollectiveAttr.o fileWriter.o GeomPrintReducer.o pointNodeParser.o AllObjectsCommBuffer_DerivedType.o  MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o adlb_mpigis_static.o
	$(CC) -O0 $(DEBUG) $(LFLAGS) -o lbStatic  fileWriter.o pointNodeParser.o GeomPrintReducer.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o adlb_mpigis_static.o $(LIBB)

adlb_mpigis_static.o : adlb_mpigis_static.cpp
	$(CC) $(CFLAGS) adlb_mpigis_static.cpp

adlb_mpigis_dyn.o: adlb_mpigis_dyn.cpp
	$(CC) $(CFLAGS) adlb_mpigis_dyn.cpp

seqWriteMBR: fileRead.o seqReadWriteGeomToMBR.o WKTParser.o FileSplits.o config.o loadCalculationByIndex.o index.o join.o
	$(CC) -O2 $(LFLAGS) -o seqWriteMBR seqReadWriteGeomToMBR.o fileRead.o WKTParser.o FileSplits.o config.o loadCalculationByIndex.o index.o join.o $(LIBB)

allobj:  pointNodeParser.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o fileRead.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o QuadTreeBasedGrid.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o adaptiveDriver.o
	$(CC) -O2 $(LFLAGS) -o allobj pointNodeParser.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o fileRead.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o QuadTreeBasedGrid.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o adaptiveDriver.o $(LIBB) $(LIBRA)
	
adaptiveGrid: fileRead.o pointNodeParser.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o QuadTreeBasedGrid.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o adaptiveDriver.o
	$(CC) -O2 $(LFLAGS) -o adaptiveGrid  pointNodeParser.o fileRead.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o QuadTreeBasedGrid.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o adaptiveDriver.o $(LIBB) $(LIBRA)

gpu: CudaDriver.o CudaJoinInterface.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o 
	$(CC) -O2 $(LFLAGS) -o gpu CudaDriver.o CudaJoinInterface.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o $(LIBB) $(LIBRA)

timing: bench_commtime.o pointNodeParser.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o
	$(CC) -O2 $(LFLAGS) -o timing pointNodeParser.o bench_commtime.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o $(LIBB) $(LIBRA)

mbr: MPI_ReadBinaryMBRFile.o pointNodeParser.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o mbrJoinDriver.o
	$(CC) -O2 $(LFLAGS) -o mbr pointNodeParser.o mbrJoinDriver.o MPI_ReadBinaryMBRFile.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o $(LIBB) $(LIBRA)

singlempi: AllObjectsCollectiveAttr.o pointNodeParser.o AllObjectsCommBuffer_DerivedType.o  MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o SingleProcessDriver.o
	$(CC) -O0 $(DEBUG) $(LFLAGS) -o singlempi pointNodeParser.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o SingleProcessDriver.o $(LIBB)

testsingle: AllObjectsCollectiveAttr.o pointNodeParser.o AllObjectsCommBuffer_DerivedType.o  MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o testSequentialDriver.o
	$(CC) -O0 $(DEBUG) $(LFLAGS) -o single pointNodeParser.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o testSequentialDriver.o $(LIBB)

grid1: GeomPrintReducer.o fileWriter.o AllObjectsCollectiveAttr.o pointNodeParser.o AllObjectsCommBuffer_DerivedType.o  MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o driver.o
	$(CC) -O0 $(DEBUG) $(LFLAGS) -o grid GeomPrintReducer.o pointNodeParser.o fileWriter.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o driver.o $(LIBB)


grid: GeomPrintReducer.o fileWriter.o AllObjectsCollectiveAttr.o pointNodeParser.o AllObjectsCommBuffer_DerivedType.o  MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o driver.o
	$(CC) -O0 $(DEBUG) $(LFLAGS) -o grid GeomPrintReducer.o pointNodeParser.o fileWriter.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o driver.o $(LIBB)

mpe: AllObjectsCollectiveAttr.o pointNodeParser.o AllObjectsCommBuffer_DerivedType.o  MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o driver.o
	$(CC) -O0 $(LFLAGS) -o grid pointNodeParser.o AllObjectsCollectiveAttr.o AllObjectsCommBuffer_DerivedType.o MPI_LargeFile_Partitioner.o MultiRoundOneLayerBufferMBR.o MultiRoundOneLayerGeomBufferMgr.o fileRead.o SingleLayerMBRBuffer.o SingleLayerGeomBufferMgr.o CollectiveAttributes.o bufferRoadNetwork.o bufferManagerGeom.o mrdriver.o EnvelopeReducer.o GeomJoinReducer.o uniformGrid.o bufferManagerMBR.o spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinStrategy.o driver.o $(LIBB)	

AllObjectsCommBuffer_DerivedType.o: AllObjectsCommBuffer_DerivedType.cpp
	$(CC) $(CFLAGS) AllObjectsCommBuffer_DerivedType.cpp
	
types: spatialTypes.o testMPITypes.o
	$(CC) $(LFLAGS) -o types spatialTypes.o testMPITypes.o $(LIBB) $(LIBRA)
	 	
io: sequentialIO.o overlay.o join.o index.o WKTParser.o FileSplits.o config.o
	$(CC) $(LFLAGS) -o io sequentialIO.o overlay.o join.o index.o WKTParser.o FileSplits.o config.o $(LIBB) $(LIBRA)

sampling: fileWriter.o SeqFilePartitioner.o road_network_parser.o WKTParser.o FileSplits.o
	$(CC) $(DEBUG) $(LFLAGS) -o sampling fileWriter.o SeqFilePartitioner.o road_network_parser.o WKTParser.o FileSplits.o $(LIBB)

largeIO: SeqFilePartitioner.o fileWriter.o road_network_parser.o FileSplits.o
	$(CC) $(DEBUG) $(LFLAGS) -o largeIO SeqFilePartitioner.o fileWriter.o road_network_parser.o FileSplits.o $(LIBB)

splitIO: sequentialIO.o SeqFilePartitioner.o road_network_parser.o FileSplits.o
	$(CC) $(DEBUG) $(LFLAGS) -o splitIO sequentialIO.o SeqFilePartitioner.o road_network_parser.o FileSplits.o $(LIBB)

filter.o: filter.cpp
	$(CC)  $(DEBUG) $(CFLAGS) filter.cpp
	
loadCalculationByIndex.o: loadCalculationByIndex.cpp
	$(CC)  $(DEBUG) $(CFLAGS) loadCalculationByIndex.cpp
	
seqReadWriteGeomToMBR.o: seqReadWriteGeomToMBR.cpp
	$(CC)  $(DEBUG) $(CFLAGS) seqReadWriteGeomToMBR.cpp
	
sequentialIO.o: sequentialIO.cpp
	$(CC)  $(DEBUG) $(CFLAGS) sequentialIO.cpp

mbrJoinDriver.o: mbrJoinDriver.cpp
	$(CC)  $(DEBUG) $(CFLAGS) mbrJoinDriver.cpp
	
CudaDriver.o: CudaDriver.cpp
	$(CC)  $(DEBUG) $(CFLAGS) CudaDriver.cpp
	
CudaJoinInterface.o: CudaJoinInterface.cpp
	$(CC)  $(DEBUG) $(CFLAGS) CudaJoinInterface.cpp
	
bench_commtime.o: bench_commtime.cpp
	$(CC)  $(DEBUG) $(CFLAGS) bench_commtime.cpp

SingleLayerMBRBuffer.o: SingleLayerMBRBuffer.cpp
	$(CC)  $(DEBUG) $(CFLAGS) SingleLayerMBRBuffer.cpp

SingleLayerGeomBufferMgr.o: SingleLayerGeomBufferMgr.cpp
	$(CC)  $(DEBUG) $(CFLAGS) SingleLayerGeomBufferMgr.cpp
	
SeqFilePartitioner.o: SeqFilePartitioner.cpp
	$(CC) $(DEBUG) $(CFLAGS) SeqFilePartitioner.cpp
	
mpioverlay: ShapeFilePartitioner.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinTaskMap.o overlaydriver.o	
	$(CC) $(LFLAGS) -o mpiover overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinTaskMap.o overlaydriver.o $(LIBB) $(LIBRA)

mpiio: spatialTypes.o util.o ShapeFilePartitioner.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinTaskMap.o driver.o
	$(CC) -O2 $(LFLAGS) -o mpi spatialTypes.o util.o road_network_parser.o overlay.o join.o index.o MPI_File_Partitioner.o WKTParser.o FileSplits.o config.o roundRobinTaskMap.o driver.o $(LIBB) $(LIBRA)

AllObjectsCollectiveAttr.o:	AllObjectsCollectiveAttr.cpp
	$(CC) $(CFLAGS) AllObjectsCollectiveAttr.cpp
	
EnvelopeReducer.o: EnvelopeReducer.cpp
	$(CC) $(CFLAGS) EnvelopeReducer.cpp
	
GeomJoinReducer.o: GeomJoinReducer.cpp
	$(CC) $(CFLAGS) $(DEBUG) GeomJoinReducer.cpp
			
testMPITypes.o: testMPITypes.cpp
	$(CC) -fpermissive $(CFLAGS) testMPITypes.cpp
	
spatialTypes.o: spatialTypes.cpp
	$(CC) -fpermissive $(CFLAGS) spatialTypes.cpp

util.o: util.cpp
	$(CC) $(CFLAGS) util.cpp

pointNodeParser.o: pointNodeParser.cpp
	$(CC) $(DEBUG) $(CFLAGS) pointNodeParser.cpp	
	
road_network_parser.o: road_network_parser.cpp
	$(CC) $(DEBUG) $(CFLAGS) road_network_parser.cpp

mrdriver.o: mrdriver.cpp
	$(CC) $(CFLAGS) $(DEBUG) mrdriver.cpp

CollectiveAttributes.o: CollectiveAttributes.cpp
	$(CC) $(CFLAGS) $(DEBUG) CollectiveAttributes.cpp

bufferRoadNetwork.o: bufferRoadNetwork.cpp
	$(CC) $(CFLAGS) $(DEBUG) bufferRoadNetwork.cpp
				
bufferManagerGeom.o: bufferManagerGeom.cpp
	$(CC) $(CFLAGS) $(DEBUG) bufferManagerGeom.cpp

bufferManagerMBR.o: bufferManagerMBR.cpp
	$(CC) $(CFLAGS) $(DEBUG) bufferManagerMBR.cpp

MultiRoundOneLayerBufferMBR.o:	MultiRoundOneLayerBufferMBR.cpp
	$(CC) $(CFLAGS) $(DEBUG) MultiRoundOneLayerBufferMBR.cpp

MultiRoundOneLayerGeomBufferMgr.o:	MultiRoundOneLayerGeomBufferMgr.cpp
	$(CC) $(CFLAGS) MultiRoundOneLayerGeomBufferMgr.cpp
	
overlay.o: overlay.cpp
	$(CC) $(CFLAGS) overlay.cpp

testSequentialDriver.o: testSequentialDriver.cpp
	$(CC) $(CFLAGS) testSequentialDriver.cpp
		
join.o: join.cpp
	$(CC) $(CFLAGS) $(DEBUG) join.cpp

index.o: index.cpp
	$(CC) $(CFLAGS) $(DEBUG) index.cpp
	
FileSplits.o: FileSplits.cpp
	$(CC) $(CFLAGS) $(DEBUG) FileSplits.cpp

mpibinary: MPI_ReadBinaryMBRFile.o MPIReadBinMBRStruct.o spatialTypes.o MPIReadBinaryAllPointNodes.o seqOSMJoin.o WKTParser.o FileSplits.o index.o join.o
	$(CC) $(LFLAGS) $(DEBUG) -o mpibinary  seqOSMJoin.o MPIReadBinMBRStruct.o spatialTypes.o MPIReadBinaryAllPointNodes.o FileSplits.o index.o join.o WKTParser.o MPI_ReadBinaryMBRFile.o $(LIBB) $(LIBRA)

MPIReadBinMBRStruct.o: MPIReadBinMBRStruct.cpp
	$(CC) $(CFLAGS) $(DEBUG) MPIReadBinMBRStruct.cpp
	
MPIReadBinaryAllPointNodes.o: MPIReadBinaryAllPointNodes.cpp
	$(CC) $(CFLAGS) $(DEBUG) MPIReadBinaryAllPointNodes.cpp	
	
MPI_ReadBinaryMBRFile.o: MPI_ReadBinaryMBRFile.cpp 
	$(CC) $(CFLAGS) $(DEBUG) MPI_ReadBinaryMBRFile.cpp
	
MPI_LargeFile_Partitioner.o: MPI_LargeFile_Partitioner.cpp 
	$(CC) $(CFLAGS) $(DEBUG) MPI_LargeFile_Partitioner.cpp
	
MPI_File_Partitioner.o: MPI_File_Partitioner.cpp 
	$(CC) $(CFLAGS) $(DEBUG) MPI_File_Partitioner.cpp

WKTParser.o: WKTParser.cpp 
	$(CC) $(CFLAGS) $(DEBUG) WKTParser.cpp
	
ShapeFilePartitioner.o: ShapeFilePartitioner.cpp 
	$(CC) $(CFLAGS) ShapeFilePartitioner.cpp
	
config.o: config.cpp 
	$(CC) $(CFLAGS) config.cpp

QuadTreeBasedGrid.o: QuadTreeBasedGrid.cpp
	$(CC) $(CFLAGS) QuadTreeBasedGrid.cpp
	
uniformGrid.o: uniformGrid.cpp 
	$(CC) $(CFLAGS) uniformGrid.cpp

roundRobinStrategy.o: roundRobinStrategy.cpp	
	$(CC) $(CFLAGS) roundRobinStrategy.cpp	

RtreeStructure.o: RtreeStructure.cpp
	$(CC) $(CFLAGS) RtreeStructure.cpp

adaptiveDriver.o: adaptiveDriver.cpp
	$(CC) $(CFLAGS) adaptiveDriver.cpp
	
driver.o: driver.cpp
	$(CC) $(CFLAGS) $(DEBUG) driver.cpp

GeomPrintReducer.o: GeomPrintReducer.cpp
		$(CC) $(CFLAGS) $(DEBUG) GeomPrintReducer.cpp
		
SingleProcessDriver.o: SingleProcessDriver.cpp
	$(CC) $(CFLAGS) $(DEBUG) SingleProcessDriver.cpp

overlaydriver.o: overlaydriver.cpp
	$(CC) $(CFLAGS) overlaydriver.cpp

fileRead.o: fileRead.cpp
	$(CC) $(CFLAGS) fileRead.cpp

fileWriter.o: fileWriter.cpp
	$(CC) $(CFLAGS) fileWriter.cpp	

seqJoin: seqOSMJoin.o overlay.o join.o index.o WKTParser.o FileSplits.o config.o
	$(CC) $(LFLAGS) -o io seqOSMJoin.o overlay.o join.o index.o WKTParser.o FileSplits.o config.o $(LIBB) $(LIBRA)

seqOSMJoin.o: seqOSMJoin.cpp
	$(CC) $(CFLAGS) seqOSMJoin.cpp
	
clean:
	rm *o grid
 
