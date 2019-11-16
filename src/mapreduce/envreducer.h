#ifndef __ENVREDUCER_H_INCLUDED__
#define __ENVREDUCER_H_INCLUDED__

#include "reduce.h"

class EnvelopeReducer : public Reducer
{
  public:
  
  int reduce(int cellId, vector<GeomInfo*>* col1, vector<GeomInfo*>* col2);
  
  int reduce(int cellId, list<Geometry*>* col1, list<Geometry*>* col2)
  {
    return -1;
  }
};

#endif
