#ifndef __WKTPARSER_H_INCLUDED__
#define __WKTPARSER_H_INCLUDED__

#include "Parser.h"
//#include "../filePartition/FileSplits.h"
#include <geos/io/WKTReader.h>
#include <geos/geom/Geometry.h>
#include <cstring>
#include <vector>
#include <tuple>
#include <fstream>
#include <map>

#include "../filePartition/config.h"
#include "../mpiTypes/mpitype.h"
#include "../bufferManager/allobjectsCollectiveAttr.h"
#include "../bufferManager/collectiveAttributes.h"

class WKTParser : public Parser 
{

  bool checkForEmptyCollection(Geometry *iGeom);
  
  list<Geometry*>* parseGeometryFromLayer( pair< bbox*, int> *l1BoxPair, pair<char *,int>* l1RecvBuf);
  map<int, list<Geometry*>* >* parseGeomFromLayerGrpByCell( pair< bbox*, int> *l1BoxPair,
																		 pair<char *,int>* l1RecvBuf);	
  
  public:
  pair<int, string*> extract(const string &string);
  
  Geometry* parseString( const pair<int, string*> &p);
  
  list<Geometry*>* parse(const FileSplits &split);
  
  list<Geometry*>* parseGeoms(const FileSplits &split);
  
  list<Geometry*>* readOSM(char* filename);
  
  list<Geometry*>* parseGeometryFromRecvBuffer( pair< bbox*, int>* bBoxPair, 
															pair<char *,int>* recvBufPair); 
  
  pair<list<Geometry*>*, list<Geometry*>* >* parseGeometryFromRecvBuffer( pair< pair< bbox*, int>* , pair< bbox*, int>* >* bBoxPair,
														   pair<pair<char *,int>*, pair<char *,int>* >*recvBufPair);
	
  map<int, list<Geometry*>* >* parseGeomFromRecvGrpByCellId( pair< bbox*, int>* bBoxPair,
  														pair<char *,int>* recvBufPair); 	
																		 
  pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >* parseGeomFromRecvGrpByCellId( pair< pair< bbox*, int>* ,
									 pair< bbox*, int>* >* bBoxPair, pair<pair<char *,int>*, pair<char *,int>* >*recvBufPair); 													    

  // map<int, list<Geometry*>* >* parseGeomFromRecvAllObj( pair< bbox*, int>* l1BoxPair,
//   														pair<char *,long>* l1RecvBuf); 

  map<int, list<Geometry*>* >* parseGeomByProcess( CollectiveAttributes *mbrAttr, pair< bbox*, int> *l1BoxPair, 
  								pair<char *,long>* l1RecvBuf, AllObjectsCollectiveAttr* geomAttr, 
									Config *args, int PACKING_LEN);

  map<int, list<Geometry*>* >*  parseGeomFromLayerAllObj( pair< bbox*, int> *l1BoxPair,
																		 pair<char *,long>* l1RecvBuf);
  ~WKTParser() {}
   
};

#endif 