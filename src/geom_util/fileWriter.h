#ifndef FILEWRITER_H
#define FILEWRITER_H


#include<iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <list>

#include <geos/geom/Geometry.h>

using namespace std;
using namespace geos::geom;

class FileWriter
{
   public:
   void writeToFile(char *filename, string *KLines, int k);
   void writeToFile(const char *filename, list<Geometry*>* geoms);
};

#endif