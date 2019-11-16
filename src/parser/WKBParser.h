#ifndef __WKBPARSER_H_INCLUDED__
#define __WKBPARSER_H_INCLUDED__

#include "Parser.h"
//#include "../filePartition/FileSplits.h"
#include <geos/io/WKBReader.h>
#include <geos/io/WKBWriter.h>
#include <geos/geom/Geometry.h>
#include <cstring>
#include <vector>
#include <tuple>
#include <fstream>
#include <map>

#include "../mpiTypes/mpitype.h"

class WKBParser : public Parser 
{

  //bool checkForEmptyCollection(Geometry *iGeom);
  
//   list<Geometry*>* parseGeometryFromLayer( pair< bbox*, int> *l1BoxPair, pair<char *,int>* l1RecvBuf);
//   
//   map<int, list<Geometry*>* >* parseGeomFromLayerGrpByCell( pair< bbox*, int> *l1BoxPair,
// 																		 pair<char *,int>* l1RecvBuf);	
  
  public:
  pair<int, string*> extract(const string &string);
  
  Geometry* parseString( const pair<int, string*> &p);
  
  list<Geometry*>* parse(const FileSplits &split);
  //list<Geometry*>* readOSM(char* filename);
  
  ~WKBParser() {}
   
};

#endif 