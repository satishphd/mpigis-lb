

// obtaining file size
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "filePartition/SeqFilePartitioner.h"

using namespace std;


void SeqFilePartitioner :: printBlock(char *mem, long int beg, long int end, long int stepSize) 
{
   for(long int i = beg; i<end; i = i+ stepSize) {
     cout<<mem[i];
   }
   cout<<endl;
}

FileSplits *  SeqFilePartitioner :: formatBlockByLines(char *memBlock)
{   
    FileSplits *split = new FileSplits();
    
    split->write(memBlock);
    
    return split;
}

char*  SeqFilePartitioner :: readBlock(ifstream *myfile, long int offset, long int size)
{
    myfile->seekg(offset, ios::beg);
    
    char *memblock = new (nothrow) char[size+1];
    
    if(!memblock)
      cout<<" ****** Memory allocation failed in readBlock method "<<endl;
      
    myfile->read(memblock, size);
    
    memblock[size] = '\0';
    
    return memblock;
}

long int SeqFilePartitioner :: countLineBreaks(char *mem, long int blockSize) {
  long int count = 0;
  for(long int i = 0; i<blockSize; i++) {
      if('\n' == mem[i])
         count++;
  }
  return count;
}

FileSplits SeqFilePartitioner :: createFileSplits(char *memoryBlock)
{
  return FileSplits();
}
    
/*int SeqFilePartitioner :: initialize(Config &args)
{
   return 0;
}*/

// void selectKItems(int stream[], int n, int k)
// {
// 	int i; // index for elements in stream[]
// 
// 	// reservoir[] is the output array. Initialize it with
// 	// first k elements from stream[]
// 	int reservoir[k];
// 	for (i = 0; i < k; i++)
// 		reservoir[i] = stream[i];
// 
// 	// Use a different seed value so that we don't get
// 	// same result each time we run this program
// 	srand(time(NULL));
// 
// 	// Iterate from the (k+1)th element to nth element
// 	for (; i < n; i++)
// 	{
// 		// Pick a random index from 0 to i.
// 		int j = rand() % (i+1);
// 
// 		// If the randomly picked index is smaller than k, then replace
// 		// the element present at the index with new element from stream
// 		if (j < k)
// 			reservoir[j] = stream[i];
// 	}
// 
// 	printf("Following are k randomly selected items \n");
// 	printArray(reservoir, k);
// }

 
 void SeqFilePartitioner ::reserveSampling(int numBlocks, char *filename, int k)
 {
    // Use a different seed value so that we don't get
	// same result each time we run this program
	srand(time(NULL));
	
	string *samples = new string[k];
	
 	streampos begin,end;
  
    //list<FileSplits*> *splits = new list<FileSplits*>();
  
   //"/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/cemetery/cemetery.csv"
   ifstream myfile (filename);
   
   int line_counter = 0;
   
   if (myfile.is_open())
   {
   	 begin = myfile.tellg();
     myfile.seekg (0, ios::end);
     end = myfile.tellg();
  
     long int size = (end-begin);
     long int partitionsize = size/numBlocks;
   
     for(int i = 0; i< numBlocks; i++) {
          
        myfile.seekg (0, ios::beg);
       
        long int start_offset = i * partitionsize;
        long int end_offset = (i+1) * partitionsize;

        if(i== (numBlocks-1)) {
           end_offset = size - 1;
        }
       
        long int blockSize = end_offset - start_offset;
        
       //cout<<i<<" block"<<" from "<<start_offset<<" to "<<end_offset<<" : "<<blockSize;
       //cout<<endl;
       
        char *memblock = readBlock(&myfile, start_offset, blockSize);
      
        FileSplits *split = formatBlockByLines(memblock);
       
        list<string> *contents = split->getContents();
        //split->printK(20);
        
        list<string>::const_iterator c;
        
        for(c = contents->begin(); c != contents->end(); c++) {
               // Pick a random index from 0 to i.
			   int j = rand() % (line_counter + 1);
				//cout<< *c<<"  ";
 			   
 			   if(line_counter < k)
 			   	  samples[line_counter] = *c;
 			   	  
			   // If the randomly picked index is smaller than k, then replace
			   // the element present at the index with new element from stream
			   if (j < k) {
			      samples[j] = *c;
			      
       		   }
               
               line_counter++;
        }
        split->clear();
        delete split;    
        delete[] memblock;
     }
    myfile.close();
    cout << "size is: " << size << " bytes.\n";

    //print(samples, k);
    FileWriter writer;
    writer.writeToFile("samples.txt", samples, k);
   }
   else 
    cout << "Unable to open file"; 
 }
 
 
 void SeqFilePartitioner :: print(string *samples, int k)
 {
   for(int i = 0; i<k; i++) {
     cout<<samples[i]<<endl<<endl;
   }
 }
 
 void SeqFilePartitioner :: seq_partition(int numBlocks, char *filename) {
  
  	char *outputFilename = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/sample_road_network/roadnetworkMBRs.txt";
    FILE *ofp = fopen(outputFilename, "w");

    if (ofp == NULL) {
       fprintf(stderr, "Can't open output file %s!\n", outputFilename);
       exit(1);
    }
  
  streampos begin,end;
  
  //list<FileSplits*> *splits = new list<FileSplits*>();
  
  //"/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/cemetery/cemetery.csv"
  ifstream myfile (filename);

  if (myfile.is_open())
  {
    begin = myfile.tellg();
    myfile.seekg (0, ios::end);
    end = myfile.tellg();
  
    long int size = (end-begin);
    long int partitionsize = size/numBlocks;
   
    for(int i = 0; i< numBlocks; i++) {
       
       #ifdef DBUGROAD
       #endif
       
       myfile.seekg (0, ios::beg);
       
       long int start_offset = i * partitionsize;
       long int end_offset = (i+1) * partitionsize;

       if(i== (numBlocks-1)) {
           end_offset = size - 1;
       }
       
       long int blockSize = end_offset - start_offset;
        
       //cout<<i<<" block"<<" from "<<start_offset<<" to "<<end_offset<<" : "<<blockSize;
       //cout<<endl;
       
       char *memblock = readBlock(&myfile, start_offset, blockSize);
      
       FileSplits *split = formatBlockByLines(memblock);
       
       road_parse(split, i, ofp);
       
       //splits->push_back(split);
       
       //cout<<"Number of lines in a split "<<split->numLines()<<endl;
       
       //if(i==7)
        //printBlock(memblock, 0, blockSize, 100);
      
       //cout<<"Line Breaks = "<<countLineBreaks(memblock, blockSize)<<endl;
       delete[] memblock;
     }
    myfile.close();
    cout << "size is: " << size << " bytes.\n";
    
   }
   else 
    cout << "Unable to open file"; 
  
   fclose(ofp);
  
   //return splits;
}

void SeqFilePartitioner :: writeToFile(list<Geometry*> *geoms, FILE *ofp )
{
  for(Geometry *geom : *geoms) {
	  const Envelope *env = geom->getEnvelopeInternal();
	  fprintf(ofp, "%f, %f, %f, %f\n", env->getMinX(), env->getMinY(), env->getMaxX(), env->getMaxY());	  
  } 	
}


 void SeqFilePartitioner :: road_parse(FileSplits *split, int blockNumber, FILE *ofp) {
   
        cout<<"Number of lines in a split "<<split->numLines()<<endl; 
        list<Geometry*> *geoms = parser->parse(*split, 0, 0);
        cout<<"# Geoms "<<geoms->size()<<" in block # "<<blockNumber<<endl;      
       
        writeToFile(geoms, ofp);
       
        split->clear();
        
        delete split;    
 
        if(geoms != NULL && geoms->size()>0)
          freeGeomsMemory(geoms);
  }
    
  void SeqFilePartitioner :: freeGeomsMemory(list<Geometry*> *geoms) {

      list<Geometry*>::const_iterator itr;
      Geometry *geom;

      for(itr = geoms->begin(); itr != geoms->end(); itr++) {
        geom = *itr;
        
        if(geom)
         delete geom;
      }
}  

int main(int argc, char *argv[]) 
{
  if(argc < 3)
  {
     cout<<"./a.out #blocks filename "<<endl;
     return 0;
  }
  
  int numBlocks = atoi(argv[1]);
  char *filename = argv[2];
  //int k = atoi(argv[3]);
 
  SeqFilePartitioner partitioner;
 
  partitioner.seq_partition(numBlocks, filename);
 
  //partitioner.reserveSampling(numBlocks, filename, k);
  return 0;
}


/*
int main () {

char * buffer;
long size;
long halfSize;
ifstream infile ("test.txt",ifstream::binary);
ofstream outfile ("new.txt",ofstream::binary);
ofstream outfile2 ("new2.txt",ofstream::binary);

// get size of file
infile.seekg(0,ifstream::end);
size=infile.tellg();
infile.seekg(0);
halfSize = static_cast<int>(floor(size/2));
// allocate memory for file content

buffer1 = new char[halfSize];
buffer2 = new char[size-halfSize];

// read content of infile
infile.read (buffer1,halfSize);
infile.seekg(halfSize+1);
 // read content of infile
infile.read (buffer2,size-halfSize);

// write to outfile
outfile.write (buffer1,halfSize);
outfile2.write (buffer2,size-halfSize);

// release dynamically-allocated memory
delete[] buffer;
delete[] buffer2;

outfile.close();
infile.close();
outfile2.close();
return 0;
}
*/