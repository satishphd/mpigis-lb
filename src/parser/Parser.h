#ifndef __PARSER_H_INCLUDED__
#define __PARSER_H_INCLUDED__

#include "../filePartition/FileSplits.h"
//#include "Parser.h"
//#include <geos/io/WKTReader.h>
//#include <geos/io/WKTWriter.h>
#include <geos/geom/Geometry.h>
//#include <geos/geom/Coordinate.h>
#include <list>
#include <tuple>

using namespace geos::geom;

class Parser 
{
  virtual Geometry* parseString(const pair<int, string*> &p) = 0;
  /* for cleaning the shape */
  
  public:
  virtual list<Geometry*>* parse(const FileSplits &split) = 0;
  virtual list<Geometry*>* parseGeoms(const FileSplits &split) = 0;
  virtual ~Parser()
  {}
};

#endif 