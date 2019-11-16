#ifndef __STATICMPILOADCALCULATOR_H_INCLUDED__
#define __STATICMPILOADCALCULATOR_H_INCLUDED__

#include "MPILoadCalculator.h"

class StaticMPILoadCalculator : public MPILoadCalculator
{

   public:
   // StaticMPILoadCalculator()
//    {
//    
//    }
   map<long, int, std::greater<long> >* calculateLoad(int numFiles, string l1Mbr_folder, string l2Mbr_folder);
   ~StaticMPILoadCalculator() {}
};

#endif