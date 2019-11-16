#include "cuda/CudaJoinInterface.h"

void CudaJoinInterface :: createReducers(Config *args, pair<map<int,list<Geometry*>* > *, map<int,list<Geometry*>* >* >* cellPairMap)
 {
 	 int cellCount = args->numPartitions;
     map<int, list<Geometry*>* > *layer1Map = cellPairMap->first;
     map<int, list<Geometry*>* > *layer2Map = cellPairMap->second;
     
     //Reducer *reducer = new GeomJoinReducer();
     
     int nonPolygons = 0;
     int *joinPairs;
     //int total = 0;
     
     for(int cellId = 0; cellId < cellCount; cellId++) {
        if(layer1Map->find(cellId) != layer1Map->end() && layer2Map->find(cellId) != layer2Map->end()) {
           
           //int output = reducer->reduce(cellId, layer1Map->at(cellId), layer2Map->at(cellId)); 
           list<Geometry*>* layer1 = layer1Map->at(cellId);
           polygonLayer* layer1Data = populateLayerData(layer1, &nonPolygons);

           //total += nonPolygons;

           list<Geometry*>* layer2 = layer2Map->at(cellId);
           polygonLayer* layer2Data = populateLayerData(layer2, &nonPolygons);
           //total += nonPolygons;
		   
		   // int** jxyVector;
// 		   int* pairNum;
		   
		   
  		   //debugCode(cellId, layer1Data, layer2Data);
  		   
  		   // ST_Intersect(long bPolNum, long oPolNum, coord_t* baseCoords, coord_t* overlayCoords, 
  		   // int* bVNum, int* oVNum, 
  		   // long *bVPSNum, long *oVPSNum, 
  		   // mbr_t* seqMBR, mbr_t* seqOMBR, coord_t *seqMBR2, coord_t* seqOMBR2)
		   
		   //ST_Intersect(long, long, float*, float*, int*, int*, long*, long*, long long*, long long*, float*, float*, int*)
		   
		   
		   try {
		        ST_Intersect(layer1Data->polNum, layer2Data->polNum, layer1Data->coord, layer2Data->coord, 
		                   layer1Data->vNum, layer2Data->vNum,
		                   layer1Data->prefixSum, layer2Data->prefixSum,
		                   layer1Data->mbr2, layer2Data->mbr2,
		                   layer1Data->mbr, layer2Data->mbr, joinPairs);	
           }catch(runtime_error &e)
			{
  				cerr << "GPU Runtime error during ST_Intersect: " << e.what() << endl;
			}
           
//           void SpatialJoin(int* bPolNum, int* oPolNum, int* bVNum, int* oVNum, coord_t* bCoord, 
//           coord_t* oCoord, coord_t * bMBR, coord_t* oMBR, mbr_t* bMBR2, mbr_t* oMBR2, int** jxyVector, int* pairNum);
        }
     }
     
     //int totalNonPolygons = 0;
     //MPI_Allreduce(&total, &totalNonPolygons, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
     //printf("Non-Polygon count = %d %d \n", nonPolygons, totalNonPolygons);
 }

 polygonLayer* CudaJoinInterface :: populateLayerData(list<Geometry*>* geoms, int *debug_param)
 {
   polygonLayer* layer = (polygonLayer*)malloc(1* sizeof(polygonLayer)); 
   
   //int *vNum = (int*)malloc(sizeof(int) * layer->polNum);
   
   vector<coord_t> *runningVector = new vector<coord_t>();
   vector<coord_t> *runningEnvVector = new vector<coord_t>();
   vector<mbr_t> *gpuEnvInLongVector = new vector<mbr_t>();
   
   vector<int> *vNumVector = new vector<int>();

   int polygonIndex = 0;
   int nonPolygons = 0;
   int numPolygons = 0;
      
   for(list<Geometry*>::iterator it = geoms->begin() ; it != geoms->end(); ++it) {
   		 Geometry *geom = *it;
   		 GeometryTypeId typeId = geom->getGeometryTypeId();
   		 
   		 switch(typeId)
   		 {
   		    case GEOS_POLYGON:
   		    {
              //( Geometry *geom, vector<coord_t> *verticesVec, vector<coord_t> *envVec)
              gpuHelperForPolygon(geom, runningVector, runningEnvVector, gpuEnvInLongVector, vNumVector);
   		  	  
   		      numPolygons++;
   		    } 
   		    break;
   		    
   		    case GEOS_MULTIPOLYGON:
   		    {
   		       gpuHelperForMultiPolygon(geom, runningVector, runningEnvVector, gpuEnvInLongVector, vNumVector);
   		 	}
   		    break;
   		    
   		    case GEOS_GEOMETRYCOLLECTION:
   		    {
   		    	size_t numGeoms = geom->getNumGeometries();
    		    numPolygons += numGeoms;   		    	
   		    	
   		    	for(size_t i = 0; i < numGeoms; i++) {
   		    	    GeometryTypeId typeId = geom->getGeometryTypeId();
   		    	  
   		    	    switch(typeId)
   		    	    {
   		    	  	 	case GEOS_POLYGON:
   		    			{
   		  				   gpuHelperForPolygon(geom, runningVector, runningEnvVector, gpuEnvInLongVector, 
   		  				   						vNumVector);
			   		       numPolygons++;
   		    		    } 
   		    		    break;
   		    
		    		 	case GEOS_MULTIPOLYGON:
   		    		 	{
		   		            gpuHelperForMultiPolygon(geom, runningVector, runningEnvVector, gpuEnvInLongVector, 
		   		            							vNumVector);
   		 			    }
   		 			    break;
   		 			      
   		 			    default: nonPolygons++;
   		 			      
   		    	     } // end inner switch
   		         } //for geom in GEOS_GEOMETRYCOLLECTION
   		      }
   		      break;
   		    
   		     default: nonPolygons++;  
   		 }
   }
   
   layer->polNum = vNumVector->size();
   layer->vNum = vNumVector->data();      //&vNumVector[0];
   
   layer->prefixSum = prefixSum(layer);

   layer->coord = runningVector->data();  //&runningVector[0];
   layer->mbr = runningEnvVector->data();
   layer->mbr2 = gpuEnvInLongVector->data();
   
   *debug_param = nonPolygons;
   //printf("filtered Polygon count %d, Non-Polygon %d argument geom count = %d \n", numPolygons, *debug_param, geoms->size());
   return layer;
 }

long* CudaJoinInterface :: prefixSum(polygonLayer *layer)
{
   long numPoly = layer->polNum;
   int* vNum = layer->vNum;
   
   long* prefixSum = (long*) malloc(sizeof(long) * numPoly);
   prefixSum[0] = vNum[0];
   
   for(long i = 1; i < numPoly; i++) {
        prefixSum[i] = prefixSum[i-1] + vNum[i];
   } 
   return prefixSum;
}
 
 void CudaJoinInterface :: gpuHelperForMultiPolygon(Geometry *geom, vector<coord_t> *verticesVec, 
 							vector<coord_t> *envVec, vector<mbr_t> *gpuEnvInLongVector, vector<int> *vNumVector)
 {
 		size_t numGeoms = geom->getNumGeometries();
   		       
   		for(size_t i = 0; i < numGeoms; i++) {
   		        const Geometry* inner = geom->getGeometryN(i);
   		          
		   		const Polygon* poly = dynamic_cast<const Polygon*>(inner); 
		   		          		
   				const LineString *innerLinestring = poly->getExteriorRing();
   				      	  		
   				vNumVector->push_back(innerLinestring->getNumPoints());
   				convertToFloats(innerLinestring, verticesVec);
   				      	  		
   				convertMBRToFloats(poly->getEnvelopeInternal(), envVec);
   				      	  		
   				convertMBRToLong(poly->getEnvelopeInternal(), gpuEnvInLongVector);
   				      	  		
   				      	  		//numPolygons++;
   		}							
 }
 
 void CudaJoinInterface :: gpuHelperForPolygon(Geometry *geom, vector<coord_t> *verticesVec, 
 							vector<coord_t> *envVec, vector<mbr_t> *gpuEnvInLongVector, vector<int> *vNumVector)
 {
    Polygon* poly = dynamic_cast<Polygon*>(geom); 
   	const LineString *linestring = poly->getExteriorRing();
   		 //LineString *linestr = const_cast<LineString*>(linestring);
   		      
   	vNumVector->push_back(linestring->getNumPoints());
   	convertToFloats(linestring, verticesVec);
   		      
	convertMBRToFloats(poly->getEnvelopeInternal(), envVec);
	
	convertMBRToLong(poly->getEnvelopeInternal(), gpuEnvInLongVector);
 }

void CudaJoinInterface :: convertMBRToLong(const Envelope* v, vector<mbr_t> *vertVect)
{
	char buffer[50];  
    
    snprintf(buffer, sizeof(buffer), "%f", v->getMinX());
    vertVect->push_back(CoordToMBR(buffer, 1));
    memset(buffer, 0, sizeof(buffer));

    snprintf(buffer, sizeof(buffer), "%f", v->getMinY());
    vertVect->push_back(CoordToMBR(buffer, 1));
    memset(buffer, 0, sizeof(buffer));
    
    snprintf(buffer, sizeof(buffer), "%f", v->getMaxX());
    vertVect->push_back(CoordToMBR(buffer, 1));
    memset(buffer, 0, sizeof(buffer));

    snprintf(buffer, sizeof(buffer), "%f", v->getMaxY());
    vertVect->push_back(CoordToMBR(buffer, 1));
    
}


void CudaJoinInterface :: convertMBRToFloats(const Envelope* v, vector<coord_t> *vertVect)
{
     vertVect->push_back((float)v->getMinX());
     vertVect->push_back((float)v->getMinY());

	 vertVect->push_back((float)v->getMaxX());
     vertVect->push_back((float)v->getMaxY());

}
 
//void convertToFloats(const LineString* vertices, vector<coord_t> *vertVect); 
void CudaJoinInterface :: convertToFloats(const LineString* vertices, vector<coord_t> *vertVect)
{
  size_t numPoints = vertices->getNumPoints();
  //coord_t* arr = (coord_t*)malloc(sizeof(coord_t)*numPoints);
  //vector<coord_t>* vertVect = new vector<coord_t>();
  
  for(size_t i = 0; i<numPoints; i++) {
     Point* pt = vertices->getPointN(i);
     vertVect->push_back((float)pt->getX());
     vertVect->push_back((float)pt->getY());

  } 
  
}


//================================ CoordToMBR ===================================
mbr_t CudaJoinInterface :: CoordToMBR(char* ct, char type){
  mbr_t retVal=0;
  const char fracNum=12;
  int fCount=0, fDot=0;
  switch(type){
    case 0:
      retVal=atof(ct);
      break;
    case 1:
      //"float" to "long long"
      char buff[50];
      int i=0;
      while(*ct){
        if(*ct!='.'){
           if(fCount<fracNum){buff[i++]=*(ct);if(fDot)fCount++;}
           else break;
        }
        else fDot=1;
        ct++;
        }
      for(;fCount<fracNum;fCount++)buff[i++]='0';
      buff[i]='\0';
      retVal=atoll(buff);
      retVal+=9000000000000000;
      //printf("\n%s %ld\n", buff, retVal);
      break;
  }
  return(retVal);
}

void CudaJoinInterface :: debugCode(int cellId, polygonLayer* layer1Data, polygonLayer* layer2Data)
{
	
	int totalVertCntLayer1 = 0;
	int* vNum = layer1Data->vNum;
	
	for(int i = 0; i<layer1Data->polNum; i++) {
	    totalVertCntLayer1 += vNum[i];
	}
	
	int totalVertCntLayer2 = 0;
	vNum = layer2Data->vNum;
	
	for(int i = 0; i<layer2Data->polNum; i++) {
	    totalVertCntLayer2 += vNum[i];
	}
	
	cout<<"Cell [ "<<cellId<<" ] "<<"Layer 1 #poly "<<layer1Data->polNum<<" Layer 2 #poly "<<layer2Data->polNum
		       <<"Layer 1 #vert "<<totalVertCntLayer1<<" Layer 2 #vert "<<totalVertCntLayer2;
	
	// mbr_t* longMBRs = layer1Data->mbr2;
// 	coord_t* mbr = layer1Data->mbr;
	
	// for(int i = 0; i<(4*layer1Data->polNum); i = i+4) {
// 	  cout<<" [ "<<mbr[i]<<" "<<mbr[i+1]<<", "<<mbr[i+2]<<" "<<mbr[i+3]<<" ] -> ";
// 	  cout<<" [ "<<longMBRs[i]<<" "<<longMBRs[i+1]<<", "<<longMBRs[i+2]<<" "<<longMBRs[i+3]<<" ]"<<endl; 
// 	}
	
	mbr_t* longMBRs = layer2Data->mbr2;
	coord_t* mbr = layer2Data->mbr;
	
	for(int i = 0; i<(4*layer2Data->polNum); i = i+4) {
	  cout<<" [ "<<mbr[i]<<" "<<mbr[i+1]<<", "<<mbr[i+2]<<" "<<mbr[i+3]<<" ] -> ";
	  cout<<" [ "<<longMBRs[i]<<" "<<longMBRs[i+1]<<", "<<longMBRs[i+2]<<" "<<longMBRs[i+3]<<" ]"<<endl; 
	}
	
	// coord_t* coords = layer1Data->coord;
// 	size_t vertOffset = 0;
// 	for(int i = 0; i<layer1Data->polNum; i++) {
// 		int vCount = layer1Data->vNum[i];
// 		cout<<"["<<i<<"] "<<vCount<<endl;
// 
//         int numCoordinates = 2*vCount;
// 		for(int j = 0; j < numCoordinates; j++) {
// 		    cout<<" "<<coords[vertOffset+j]<<" ";
// 		}
// 		vertOffset += numCoordinates;
// 		cout<<endl;
// 	}
	
	
	// coord_t* coords = layer2Data->coord;
// 	size_t vertOffset = 0;
// 	for(int i = 0; i<layer2Data->polNum; i++) {
// 		int vCount = layer2Data->vNum[i];
// 		cout<<"["<<i<<"] "<<vCount<<endl;
// 
//         int numCoordinates = 2*vCount;
// 		for(int j = 0; j < numCoordinates; j++) {
// 		    cout<<" "<<coords[vertOffset+j]<<" ";
// 		}
// 		vertOffset += numCoordinates;
// 		cout<<endl;
// 	}
}
