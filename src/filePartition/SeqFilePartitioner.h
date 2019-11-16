#ifndef __SEQ_FILEPARTITIONER_H_INCLUDED__
#define __SEQ_FILEPARTITIONER_H_INCLUDED__

//#include "FilePartitioner.h"
#include "FileSplits.h"
#include "../parser/WKTParser.h"
#include "../parser/road_network_parser.h"
#include "../geom_util/fileWriter.h"

//#define DBUGROAD 1

class SeqFilePartitioner //: public FilePartitioner
{

   Parser *parser;
   
   void printBlock(char *mem, long int beg, long int end, long int stepSize);
   char*  readBlock(ifstream *myfile, long int offset, long int size);
   long int countLineBreaks(char *mem, long int blockSize);
    
   FileSplits createFileSplits(char *memoryBlock);
   
   FileSplits* formatBlockByLines(char *memBlock);
    
   void road_parse(FileSplits *split, int blockNumber,FILE *ofp);
    
   void freeGeomsMemory(list<Geometry*> *geoms);
    
   void print(string *samples, int k); 
    
   void writeToFile(list<Geometry*> *geoms, FILE *ofp); 
 
   public:
   
    SeqFilePartitioner() 
    {
       parser = new RoadNetworkParser();
       //parser = new WKTParser();
    }
   
    pair<FileSplits*,FileSplits*> partition()
    { 
      pair<FileSplits*, FileSplits*> p(NULL, NULL);
      return p;
    }
	
    //int initialize(Config &args); 
    
    //list<FileSplits*>* seq_partition(int numBlocks, char *filename); 
    void seq_partition(int numBlocks, char *filename); 
    
    void reserveSampling(int numBlocks, char *filename, int k);
    
    //void finalize(){}
    
    ~SeqFilePartitioner()
    {
       //outputFile.close();
    }
};

#endif 