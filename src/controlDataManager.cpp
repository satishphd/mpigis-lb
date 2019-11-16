#include "bufferManager/controlDataManager.h"

/* This is required for MPI_AllToAllv communication 
   because a process does not know how many it will receive from other fellow processes
*/
pair<vector<GeomInfo>*, vector<GeomInfo>* >* ControlDataMgr :: getAllToAllControlData(MappingStrategy *strategy, Grid *grid, Config *args)
{
   int numProcesses = args->numProcesses;
   
   map<int, vector<int>* > *processToCellsMapping = strategy->getProcessToCellsMap();
   
   map<int, Cell*> *shapeInACell = grid->getShapesInCell();
   
   geos::io::WKTWriter wktWriter;
   
   //list<string> *polyStrL1 = new list<string>();
   //list<string> *polyStrL2 = new list<string>();
   
   vector<GeomInfo> *geomL1Info = new vector<GeomInfo>();
   vector<GeomInfo> *geomL2Info = new vector<GeomInfo>();
   
//    long layer1VCntForP = 0;
//    long layer2VCntForP = 0;

   // Iterate through processToCellsMapping and add the shapes up using shapeInACell 
   
   for(int pid = 0; pid < numProcesses; pid++) {
   	   
       vector<int> *cells = processToCellsMapping->at(pid);
         	  
       for(int cellId : *cells) {
          Cell *cell = shapeInACell->at(cellId);
          list<Geometry*> *layerAGeom = cell->getLayerAGeom();
          
          for(list<Geometry*>::iterator it = layerAGeom->begin() ; it != layerAGeom->end(); ++it) {
             Geometry *geom = *it;
             string str1 = wktWriter.write(geom);
             
             GeomInfo l1Info;
             l1Info.length = str1.size();
             l1Info.cellId = cellId;
             
             geomL1Info->push_back(l1Info);
             
             //polyStrL1->push_back(str1);
             //layer1VCntForP += str1.size();
             
          }
          
           list<Geometry*> *layerBGeom = cell->getLayerBGeom();
           
           for(list<Geometry*>::iterator it = layerBGeom->begin() ; it != layerBGeom->end(); ++it) {
             Geometry *geom = *it;
             string str2 = wktWriter.write(geom);
             
             GeomInfo l2Info;
             l2Info.length = str2.size();
             l2Info.cellId = cellId;
             geomL2Info->push_back(l2Info);
             
             //polyStrL2->push_back(str2);
             //layer2VCntForP += str2.size();
          } 
       }
   }
   return new pair<vector<GeomInfo>*, vector<GeomInfo>* >(geomL1Info, geomL2Info);
}

pair<GeomInfo*, int>* ControlDataMgr :: exchangeControlData(GeomInfo *send_GeomInfo, CollectiveAttributes *attr) 
{
    SpatialTypes types;
    MPI_Datatype MPI_GEOM_INFO = types.createGeomInfoType();
    GeomInfo* recv_GeomInfo = new GeomInfo[attr->recvBufSize]; //(struct bbox *)malloc(attr->recvBufSize * sizeof(struct bbox*));
	
	MPI_Alltoallv(send_GeomInfo, attr->sendCountsArr, attr->sdispls, MPI_GEOM_INFO, 
	recv_GeomInfo, attr->recvCountArr, attr->rdispls, MPI_GEOM_INFO, MPI_COMM_WORLD);
	
	return new pair<GeomInfo*, int>(recv_GeomInfo, attr->recvBufSize);
	
	//printResults(recvBaseBuffer, attr->recvBufSize);
}

  pair<vector<GeomInfo>*, vector<GeomInfo>* >*  ControlDataMgr :: shuffleExchangeCntrlData( 
  																   pair<vector<GeomInfo>*, vector<GeomInfo>* >* geomInfo,
                                                                   pair<CollectiveAttributes*, CollectiveAttributes*>* collectiveAttr)
{
      // for layer 1
      vector<GeomInfo>* layer1GeomInfo = geomInfo->first;
      GeomInfo *infoArr1 = layer1GeomInfo->data();
      
      pair<GeomInfo*, int>* l1GeomInfo = exchangeControlData(infoArr1, collectiveAttr->first);

      // for layer 2
      vector<GeomInfo>* layer2GeomInfo = geomInfo->second;
      GeomInfo *infoArr2 = layer2GeomInfo->data();
      
      pair<GeomInfo*, int>* l2GeomInfo = exchangeControlData(infoArr2, collectiveAttr->second);
      
     
//       map<int,vector<Envelope*>* > *envelopesByCellId1= parseEnvelopesFromStruct(myEnvelopesL1);            
 
//       return new pair<map<int,vector<Envelope*>* > *, map<int,vector<Envelope*>* > *>(envelopesByCellId1, envelopesByCellId2); 
    return nullptr;
}



 // stringstream ss;
//              ss << cellId <<":";
//              string cellIdStr = ss.str();
//              str2 = str2 + cellIdStr;