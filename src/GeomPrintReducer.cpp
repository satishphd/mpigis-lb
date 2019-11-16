#include "mapreduce/geomprintreducer.h"
#include "geom_util/fileWriter.h"

/* For each cell, opens two files and write into them */
int GeomPrintReducer :: reduce(int cellId, list<Geometry*>* col1, list<Geometry*>* col2)
{
    //cout<<col1->size()<<","<<col2->size()<<endl;
   // std::string path = "/home/satish/mpigis/indexed_data/cemetery/";
    //std::string path = "/home/satish/mpigis/indexed_data/lakes/64Parts/";
    std::string path = "/home/satish/mpigis/indexed_data/roads25GB/2048Parts/";
    //std::string path = "/home/satish/mpigis/indexed_data/roads25GB/4096Parts/";
    
    //std::string path = "/home/satish/mpigis/indexed_data/lakes/8192Parts/";
    
    std::string cellStr = std::to_string(cellId); 
    std::string fileName = path + cellStr;

    FileWriter writer;
    const char *file = fileName.c_str();
    writer.writeToFile(file, col1);
    cout<<"write"<<endl;
	return 0;
}