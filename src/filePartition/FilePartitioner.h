#ifndef __FILEPARTITIONER_H_INCLUDED__
#define __FILEPARTITIONER_H_INCLUDED__

#include "config.h"
#include <list>
#include <string>
#include <tuple>

#include "FileSplits.h"

using namespace std;

class FilePartitioner 
{
	int numPartitions;
	int numLines; // lines in input file
    list<string> files;
	
	public:
	
	virtual pair<FileSplits*,FileSplits*> partition() = 0; 
	
	void setPartition(int num)
	{
    	numPartitions = num;
	}
	
	virtual int initialize(Config &args) = 0;
	virtual void finalize() = 0;
	virtual ~FilePartitioner()
	{
	}
};

#endif 