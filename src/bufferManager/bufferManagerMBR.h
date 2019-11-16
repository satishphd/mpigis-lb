#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "../filePartition/config.h"
#include "../spatialPartition/grid.h"
#include "../taskMap/TaskMap.h"
#include "collectiveAttributes.h"
#include "../mpiTypes/mpitype.h"
#include "../parser/WKTParser.h"

#include<assert.h>
#include<vector>
#include<map>
#include<tuple>
#include<geos/geom/Geometry.h>
#include<sstream>

//#include <geos/io/WKTReader.h>
//#include <geos/io/WKTWriter.h>

using namespace std;
using namespace geos::geom;	

class MPI_BufferManager
{
    //map<int, Envelope*> *grid;
    //map<int, Cell*> *aShapesInCell;
    //map<int, vector<int>* > *processToCellsMapping;

    protected:

    Grid *grid;
    MappingStrategy *strategy;
    Config *args;
    
    pair<CollectiveAttributes*, CollectiveAttributes*>* m_attr;
    
    pair<CollectiveAttributes*, CollectiveAttributes*>* getAllToAllData();
    
    int calcSendBufferL1ShpCnt(int *sendBuffer, int nprocs);
    int calcSendBufferL2ShpCnt(int *sendBuffer, int nprocs);
    
    CollectiveAttributes* populateL1CollectiveAttributes(int *sendBuffer, int *recvBuffer, int nprocs);

    CollectiveAttributes* populateL2CollectiveAttributes(int *sendBuffer, int *recvBuffer, int nprocs);
    
    int* prefixSum(int *orig, int length);                                         

    int *popRecvL1Args(int *recvShpCntBuffer, int nprocs, int *aTotalReceiveBaseBoxes);
    int *popRecvL2Args(int *recvShpCntBuffer, int nprocs, int *aTotalReceiveBaseBoxes);    
     
    void printCollectiveAttributes(CollectiveAttributes *attr, int numProcesses);    
    
    bbox* populateL1SendBuffer4Box(CollectiveAttributes *attr, Grid *grid, int numProcesses);

    bbox* populateL2SendBuffer4Box(CollectiveAttributes *attr, Grid *grid, int numProcesses);

    void packEnvelope(int cellId, list<Geometry*> *geoms, bbox *envelopes, int *envCounter);
    
    pair<bbox*, int>* exchangeEnvelopes(bbox *envelopes, CollectiveAttributes *attr);

    void printResults(bbox* recvBuffer, int length);

    //pair<CollectiveAttributes*, CollectiveAttributes*>* init();
    void init();
    
    map<int,vector<GeomInfo*>* >* parseEnvelopesFromStruct(pair<bbox*, int>* recvBoxes);
    
    public:
    
    MPI_BufferManager(MappingStrategy *strategy, Grid *grid, Config *args)
    {
      this->strategy = strategy;
      this->grid = grid;
      this->args = args;
      
      init();     
    }
    
    pair<map<int,vector<GeomInfo*>* > *, map<int,vector<GeomInfo*>* >* >* shuffleExchangeMBRGroupByCellId();
        
	pair< pair< bbox*, int>* , pair< bbox*, int>* >* shuffleExchangeMBR();
    //int numShapesPerProcess(map<int, pair<int, int> > cellToShapeCountPairs);
    
    
};

#endif