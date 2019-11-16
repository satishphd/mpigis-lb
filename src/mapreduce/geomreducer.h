#ifndef __GEOMREDUCER_H_INCLUDED__
#define __GEOMEDUCER_H_INCLUDED__

#include "reduce.h"

class GeomJoinReducer : public Reducer
{
  public:
  
  int reduce(int cellId, list<Geometry*>* col1, list<Geometry*>* col2);

  int reduce(int cellId, vector<GeomInfo*>* col1, vector<GeomInfo*>* col2)
  {
    return -2;
  }
};

#endif
