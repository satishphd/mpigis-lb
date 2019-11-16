#ifndef __SHAPEFILEPARTITIONER_H_INCLUDED__
#define __SHAPEFILEPARTITIONER_H_INCLUDED__

#include "config.h"
#include "shapefil.h"
#include "FilePartitioner.h"
#include "FileSplits.h"

class ShapeFilePartitioner : public FilePartitioner
{

  SHPHandle	hSHP;
  
  ShapeFilePartitioner()
  {}
  
  void distRead(int startIndex,int endIndex,char * filename,SHPObject	***psShape);
  void destoryObjects(SHPObject **psShape,int num);
  void printBaseObjects(SHPObject	**psShape,int num);

  public:
   
  ~ShapeFilePartitioner()
  {
  }
  
  pair<FileSplits*, FileSplits*> partition(); 
  
 int initialize(Config &args);
 
 void finalize()
 {
 	SHPClose( hSHP );
 }
	
};

#endif
