#ifndef REDUCER_H
#define REDUCER_H

#include<vector>
#include <geos/geom/Geometry.h>
#include "../index/index.h"
#include "../join/join.h"

using namespace geos::geom;
using namespace std;

class Reducer
{
  public:
  virtual int reduce(int cellId, vector<GeomInfo*>* col1, vector<GeomInfo*>* col2) = 0;
  virtual int reduce(int cellId, list<Geometry*>* col1, list<Geometry*>* col2) = 0;
};

#endif