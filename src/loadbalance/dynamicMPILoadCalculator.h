#ifndef __DYNAMICMPILOADCALCULATOR_H_INCLUDED__
#define __DYNAMICMPILOADCALCULATOR_H_INCLUDED__

#include "MPILoadCalculator.h"

class DynamicMPILoadCalculator : public MPILoadCalculator
{

   public:
   map<long, int, std::greater<long> >* calculateLoad(int numFiles, string l1Mbr_folder, string l2Mbr_folder);

   void slave(int numFiles, string l1Mbr_folder, string l2Mbr_folder);
   
   map<int, long>* master(int numFiles);

   ~DynamicMPILoadCalculator() {}
};

#endif