#include "geom_util/fileWriter.h"

void FileWriter :: writeToFile(char *filename, string *KLines, int k)
{
  ofstream myfile (filename);
  
  if (myfile.is_open())
  {
    for(int i = 0; i<k; i++) {
       myfile << KLines[i]<<endl;
    }
    
    myfile.close();
  }
  else 
    cout << "Unable to open file";
	   	
}

void FileWriter :: writeToFile(const char *filename, list<Geometry*>* geoms)
{
  ofstream myfile (filename);
  
  if (myfile.is_open())
  {
    for(Geometry *geom : *geoms) {
       string wkt = geom->toString();
       myfile << wkt<<endl;
    }
    
    myfile.close();
  }
  else 
    cout << "Unable to open file";
}