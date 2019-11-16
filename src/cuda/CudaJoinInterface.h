#ifndef CUDAJOIN_H
#define CUDAJOIN_H

#include<list>
#include<map>
#include<tuple>
#include<geos/geom/Geometry.h>
#include<geos/geom/LineString.h>
#include<geos/geom/Polygon.h>
#include<geos/geom/Point.h>
#include<vector>
#include<string.h>
#include<stdio.h>

//#include "../ST_Intersect.h"
#include "../Types.h"
#include "../filePartition/config.h"

using namespace std;
using namespace geos::geom;	

// typedef float coord_t1;
// typedef long long mbr_t1;
  
typedef struct
{
  long polNum;	   // Number of polygons in a layer
  
  int* vNum; 	   // Number of vertices in each polygon. For polNum polygons, vNum has polNum int elements.
  
  long* prefixSum;
  
  coord_t* coord;  // Vertices of all the polygons of a layer. 
      			   // Format (x1_0,y1_0, x2_0, y2_0,...., x1_2, y1_2,.....,x1_(polNum-1), y1_(polNum-1) )
  coord_t* mbr;    // MBRs from polygons of coord_t type (float). 
  				   // Format (x1, y1, x2, y2) in a vector structure of size 4 * polNum * sizeof(coord_t)
  mbr_t* mbr2;     // MBRs from polygons of mbr_t type (long long). Same format as mbr.
}polygonLayer;

int ST_Intersect(long, long, coord_t*, coord_t*,
                 int*, int*, long*, long*,
                 mbr_t*, mbr_t*, coord_t*, coord_t*, int*);
//void ST_Intersect(long bPolNum, long oPolNum, coord_t* baseCoords, coord_t* overlayCoords,
// int* bVNum, int* oVNum, long *bVPSNum, long *oVPSNum,
// mbr_t* seqMBR, mbr_t* seqOMBR, coord_t *seqMBR2, coord_t* seqOMBR2);

class CudaJoinInterface
{
 long* prefixSum(polygonLayer *layer);
 
 void convertToFloats(const LineString* vertices, vector<coord_t> *vertVect);
 void convertMBRToFloats(const Envelope* v, vector<coord_t> *vertVect);
 void convertMBRToLong(const Envelope* v, vector<mbr_t> *vertVect);
 
 void debugCode(int cellId, polygonLayer* layer1Data, polygonLayer* layer2Data);
 
 mbr_t CoordToMBR(char* ct, char type);
 
 void gpuHelperForPolygon(Geometry *geom, vector<coord_t> *verticesVec, vector<coord_t> *envVec,
					vector<mbr_t> *gpuEnvInLongVector, vector<int> *vNumVector);

 void gpuHelperForMultiPolygon(Geometry *geom, vector<coord_t> *verticesVec, vector<coord_t> *envVec,
					    vector<mbr_t> *gpuEnvInLongVector, vector<int> *vNumVector);
 
 public:
 
 polygonLayer* populateLayerData(list<Geometry*>* layer, int* debug_param);
 
 void createReducers(Config *args, pair<map<int,list<Geometry*>* > *, map<int,list<Geometry*>* >* >* cellPairMap);
 
 void SpatialJoin(polygonLayer *layer1, polygonLayer *layer2);
 // void SpatialJoin(int* bPolNum, int* oPolNum, int* bVNum, int* oVNum, coord_t* bCoord, 
//    coord_t* oCoord, coord_t * bMBR, coord_t* oMBR, mbr_t* bMBR2, mbr_t* oMBR2, int** jxyVector, int* pairNum);
};

#endif  
/*
   Input parameters:

   bPolNum: Number of polygons in layer 1

   bMBR: MBRs from polygons of coord_t type (float). Format (x1, y1, x2, y2) in a 
          vector structure of size 4*bPolNum*sizeof(coord_t)

   bMBR2: MBRs from polygons of mbr_t type (long long). Same format as bMBR.

   bVNum: Number of vertices in each polygon. We have bPolNum polygons then bVNum has bPolNum int elements.

   bCoord: Vertices of all the polygons of layer b. 
      Format (x1_0,y1_0, x2_0, y2_0,...., x1_2, y1_2,.....,x1_(bPolNum-1), y1_(bPolNum-1) )



   Output parameters:

   pairNum: Number of output pairs

   jxyVector: Output pairs in format (i1, j1, i2, j2,....) i1: index of polygon from b layer and 
              j1 index of polygon from o layer.
*/
